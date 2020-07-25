using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using CSR;
using System.Web.Script.Serialization;
namespace CSRDemo
{
	class Program {
		private static MCCSAPI mcapi = null;

		public static void init(MCCSAPI api) {
			mcapi = api;
			Console.OutputEncoding = Encoding.UTF8;
			// 后台指令监听
			api.addBeforeActListener(EventKey.onServerCmd, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var se = BaseEvent.getFrom(x) as ServerCmdEvent;
				if (se != null) {
					Console.WriteLine("后台指令={0}", se.cmd);
				}
				return true;
			});
			// 后台指令输出监听
			api.addBeforeActListener(EventKey.onServerCmdOutput, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var se = BaseEvent.getFrom(x) as ServerCmdOutputEvent;
				if (se != null) {
					Console.WriteLine("后台指令输出={0}", se.output);
				}
				return true;
			});
			// 使用物品监听
			api.addAfterActListener(EventKey.onUseItem, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var ue = BaseEvent.getFrom(x) as UseItemEvent;
				if (ue != null && ue.RESULT) {
					Console.WriteLine("玩家 {0} 在 {1} 的 ({2}, {3}, {4})" +
						" 处使用了 {5} 物品。", ue.playername, ue.dimension, ue.position.x, ue.position.y, ue.position.z, ue.itemname);
				}
				return true;
			});
			// 放置方块监听
			api.addAfterActListener(EventKey.onPlacedBlock, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var ue = BaseEvent.getFrom(x) as PlacedBlockEvent;
				if (ue != null && ue.RESULT) {
					Console.WriteLine("玩家 {0} 在 {1} 的 ({2}, {3}, {4})" +
						" 处放置了 {5} 方块。", ue.playername, ue.dimension, ue.position.x, ue.position.y, ue.position.z, ue.blockname);
				}
				return true;
			});
			// 破坏方块监听
			api.addBeforeActListener(EventKey.onDestroyBlock, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var ue = BaseEvent.getFrom(x) as DestroyBlockEvent;
				if (ue != null) {
					Console.WriteLine("玩家 {0} 试图在 {1} 的 ({2}, {3}, {4})" +
						" 处破坏 {5} 方块。", ue.playername, ue.dimension, ue.position.x, ue.position.y, ue.position.z, ue.blockname);
				}
				return true;
			});
			// 开箱监听
			api.addBeforeActListener(EventKey.onStartOpenChest, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var ue = BaseEvent.getFrom(x) as StartOpenChestEvent;
				if (ue != null) {
					Console.WriteLine("玩家 {0} 试图在 {1} 的 ({2}, {3}, {4})" +
						" 处打开 {5} 箱子。", ue.playername, ue.dimension, ue.position.x, ue.position.y, ue.position.z, ue.blockname);
				}
				return true;
			});
			// 开桶监听
			api.addBeforeActListener(EventKey.onStartOpenBarrel, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var ue = BaseEvent.getFrom(x) as StartOpenBarrelEvent;
				if (ue != null) {
					Console.WriteLine("玩家 {0} 试图在 {1} 的 ({2}, {3}, {4})" +
						" 处打开 {5} 木桶。", ue.playername, ue.dimension, ue.position.x, ue.position.y, ue.position.z, ue.blockname);
				}
				return true;
			});
			// 关箱监听
			api.addAfterActListener(EventKey.onStopOpenChest, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var ue = BaseEvent.getFrom(x) as StopOpenChestEvent;
				if (ue != null) {
					Console.WriteLine("玩家 {0} 在 {1} 的 ({2}, {3}, {4})" +
						" 处关闭 {5} 箱子。", ue.playername, ue.dimension, ue.position.x, ue.position.y, ue.position.z, ue.blockname);
				}
				return true;
			});
			// 关桶监听
			api.addAfterActListener(EventKey.onStopOpenBarrel, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var ue = BaseEvent.getFrom(x) as StopOpenBarrelEvent;
				if (ue != null) {
					Console.WriteLine("玩家 {0} 在 {1} 的 ({2}, {3}, {4})" +
						" 处关闭 {5} 木桶。", ue.playername, ue.dimension, ue.position.x, ue.position.y, ue.position.z, ue.blockname);
				}
				return true;
			});
			// 放入取出监听
			api.addAfterActListener(EventKey.onSetSlot, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as SetSlotEvent;
				if (e != null) {
					if (e.itemcount > 0)
						Console.WriteLine("玩家 {0} 在 {1} 槽放入了 {2} 个 {3} 物品。",
							e.playername, e.slot, e.itemcount, e.itemname);
					else
						Console.WriteLine("玩家 {0} 在 {1} 槽取出了物品。",
							e.playername, e.slot);
				}
				return true;
			});
			// 切换维度监听
			api.addAfterActListener(EventKey.onChangeDimension, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as ChangeDimensionEvent;
				if (e != null && e.RESULT) {
						Console.WriteLine("玩家 {0} {1} 切换维度至 {2} 的 ({3},{4},{5}) 处。",
							e.playername, e.isstand?"":"悬空地", e.dimension, e.XYZ.x, e.XYZ.y, e.XYZ.z);
				}
				return true;
			});
			// 生物死亡监听
			api.addAfterActListener(EventKey.onMobDie, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as MobDieEvent;
				if (e != null && !string.IsNullOrEmpty(e.mobname)) {
						Console.WriteLine(" {0} 在 {1} ({2:F2},{3:F2},{4:F2}) 处被 {5} 杀死了。",
							e.mobname, e.dimension, e.XYZ.x, e.XYZ.y, e.XYZ.z, e.srcname);
				}
				return true;
			});
			// 生物伤害监听
			api.addBeforeActListener(EventKey.onMobHurt, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as MobHurtEvent;
				if (e != null && !string.IsNullOrEmpty(e.mobname)) {
						Console.WriteLine(" {0} 在 {1} ({2:F2},{3:F2},{4:F2}) 即将受到来自 {5} 的 {6} 点伤害，类型 {7}",
							e.mobname, e.dimension, e.XYZ.x, e.XYZ.y, e.XYZ.z, e.srcname, e.dmcount, e.dmtype);
				}
				return true;
			});
			// 玩家重生监听
			api.addAfterActListener(EventKey.onRespawn, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as RespawnEvent;
				if (e != null && e.RESULT) {
						Console.WriteLine("玩家 {0} 已于 {1} 的 ({2:F2},{3:F2},{4:F2}) 处重生。",
							e.playername, e.dimension, e.XYZ.x, e.XYZ.y, e.XYZ.z);
				}
				return true;
			});
			// 聊天监听
			api.addAfterActListener(EventKey.onChat, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as ChatEvent;
				if (e != null) {
					Console.WriteLine(" {0} {1} 说：{2}", e.playername,
						!string.IsNullOrEmpty(e.target) ? "悄悄地对 " + e.target : "", e.msg);
				}
				return true;
			});
			// 输入文本监听
			api.addBeforeActListener(EventKey.onInputText, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as InputTextEvent;
				if (e != null) {
					Console.WriteLine(" <{0}> {1}", e.playername, e.msg);
				}
				return true;
			});
			// 更新命令方块监听
			api.addBeforeActListener(EventKey.onCommandBlockUpdate, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as CommandBlockUpdateEvent;
				if (e != null) {
					Console.WriteLine(" {0} 试图修改位于 {1} ({2},{3},{4}) 的 {5} 的命令为 {6}",
						e.playername, e.dimension, e.position.x, e.position.y, e.position.z,
						e.isblock ? "命令块" : "命令矿车", e.cmd);
				}
				return true;
			});
			// 输入指令监听
			api.addBeforeActListener(EventKey.onInputCommand, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as InputCommandEvent;
				if (e != null) {
					Console.WriteLine(" <{0}> {1}", e.playername, e.cmd);
				}
				return true;
			});
			// 命令块执行指令监听，拦截
			api.addBeforeActListener(EventKey.onBlockCmd, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as BlockCmdEvent;
				if (e != null) {
					Console.WriteLine("位于 {0} ({1},{2},{3}) 的 {4} 试图执行指令 {5}",
						e.dimension, e.position.x, e.position.y, e.position.z, e.name, e.cmd);
				}
				return false;
			});
			// NPC执行指令监听，拦截
			api.addBeforeActListener(EventKey.onNpcCmd, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as NpcCmdEvent;
				if (e != null) {
					Console.WriteLine("位于 {0} ({1},{2},{3}) 的 {4} 试图执行第 {5} 条指令，指令集\n{6}",
						e.dimension, e.position.x, e.position.y, e.position.z, e.npcname, e.actionid, e.actions);
				}
				return false;
			});
			// 世界范围爆炸监听，拦截
			api.addBeforeActListener(EventKey.onLevelExplode, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as LevelExplodeEvent;
				if (e != null) {
					Console.WriteLine("位于 {0} ({1},{2},{3}) 的 {4} 试图发生强度 {5} 的爆炸。",
						e.dimension, e.position.x, e.position.y, e.position.z,
						string.IsNullOrEmpty(e.entity) ? e.blockname : e.entity, e.explodepower);
				}
				return false;
			});
			/*
			// 玩家移动监听
			api.addAfterActListener(EventKey.onMove, x => {
				var e = BaseEvent.getFrom(x) as MoveEvent;
				if (e != null) {
					Console.WriteLine("玩家 {0} {1} 移动至 {2} ({3},{4},{5}) 处。",
						e.playername, (e.isstand) ? "":"悬空地", e.dimension,
						e.XYZ.x, e.XYZ.y, e.XYZ.z);
				}
				return false;
			});
			*/
			// 玩家加入游戏监听
			api.addAfterActListener(EventKey.onLoadName, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var ue = BaseEvent.getFrom(x) as LoadNameEvent;
				if (ue != null) {
					Console.WriteLine("玩家 {0} 加入了游戏，xuid={1}", ue.playername, ue.xuid);
				}
				return true;
			});
			// 玩家离开游戏监听
			api.addAfterActListener(EventKey.onPlayerLeft, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var ue = BaseEvent.getFrom(x) as PlayerLeftEvent;
				if (ue != null) {
					Console.WriteLine("玩家 {0} 离开了游戏，xuid={1}", ue.playername, ue.xuid);
				}
				return true;
			});
			
			// 攻击监听
			// API 方式注册监听器
			api.addAfterActListener(EventKey.onAttack, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				AttackEvent ae = BaseEvent.getFrom(x) as AttackEvent;
				if (ae != null) {
					string str = "玩家 " + ae.playername + " 在 (" + ae.XYZ.x.ToString("F2") + "," +
						ae.XYZ.y.ToString("F2") + "," + ae.XYZ.z.ToString("F2") + ") 处攻击了 " + ae.actortype + " 。";
					Console.WriteLine(str);
					//Console.WriteLine("list={0}", api.getOnLinePlayers());
					JavaScriptSerializer ser = new JavaScriptSerializer();
					ArrayList al = ser.Deserialize<ArrayList>(api.getOnLinePlayers());
					object uuid = null;
					foreach (Dictionary<string, object> p in al) {
						object name;
						if (p.TryGetValue("playername", out name)) {
							if ((string)name == ae.playername) {
								// 找到
								p.TryGetValue("uuid", out uuid);
								break;
						}
					}
					}
					if (uuid != null) {
						api.sendSimpleForm((string)uuid,
						                   "致命选项",
						                   "test choose:",
						                   "[\"生存\",\"死亡\",\"求助\"]");
						//api.transferserver((string)uuid, "www.xiafox.com", 19132);
					}
				} else {
					Console.WriteLine("Event convent fail.");
				}
				return true;
			});
			
			// Json 解析部分 使用JavaScriptSerializer序列化Dictionary或array即可

			//JavaScriptSerializer ser = new JavaScriptSerializer();
			//var data = ser.Deserialize<Dictionary<string, object>>("{\"x\":9}");
			//var ary = ser.Deserialize<ArrayList>("[\"x\",\"y\"]");
			//Console.WriteLine(data["x"]);
			//foreach(string v in ary) {
			//	Console.WriteLine(v);
			//}
			//data["y"] = 8;
			//string dstr = ser.Serialize(data);
			//Console.WriteLine(dstr);
			
			// 高级玩法，硬编码方式注册hook
			THook.init(api);
		}
	}
}

namespace CSR
{
	partial class Plugin
	{
		/// <summary>
		/// 通用调用接口，需用户自行实现
		/// </summary>
		/// <param name="api">MC相关调用方法</param>
		public static void onStart(MCCSAPI api) {
			// TODO 此接口为必要实现
			CSRDemo.Program.init(api);
		}
	}
}