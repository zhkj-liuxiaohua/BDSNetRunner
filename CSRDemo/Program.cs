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

		// 测试用的临时表单ID
		private static uint tformid;

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
            // 表单选择监听
            api.addAfterActListener(EventKey.onFormSelect, x => {
                Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
                var fe = BaseEvent.getFrom(x) as FormSelectEvent;
                if (fe == null) return true;
				if (fe.formid != tformid)
					return true;
                if (fe.selected != "null"){
                    Console.WriteLine("玩家 {0} 选择了表单 id={1} ，selected={2}", fe.playername, fe.formid, fe.selected);
                    CsPlayer pl = new CsPlayer(api, fe.playerPtr);
                    if (fe.selected == "0"){
                        ulong bkid = pl.UniqueId;
                        // 根据流水号反查，可能是实体，也可能是玩家
                        var d = CsActor.getFromUniqueId(api, bkid) as CsPlayer;
                        if (d != null){
                            Console.WriteLine("UniqueId反查成功。");
                            pl = d;
                        }
                        // 常规组件测试
                        Console.WriteLine("玩家攻击力组件：{0}，玩家碰撞箱组件：{1}，玩家生命值组件：{2}，玩家位置组件：{3}，玩家转角组件：{4}" +
                        "，玩家所处维度：{5}，玩家实体类型：{6}，玩家查询流水号：{7}，玩家UUID：{8}，玩家名字：{9}，玩家计分板ID：{10}",
                        pl.Attack, pl.CollisionBox, pl.Health, pl.Position, pl.Rotation, pl.DimensionId, pl.TypeId, pl.UniqueId, pl.Uuid, pl.getName(),
						pl.getScoreboardId()/* , pl.createScoreboardId() */);
                    }
                    else if (fe.selected == "1"){
                        // 物品栏测试
                        Console.WriteLine("实体装备栏：{0}，实体主副手栏：{1}，实体背包栏：{2}，实体热键栏：{3}",
                            pl.ArmorContainer, pl.HandContainer, pl.InventoryContainer, pl.HotbarContainer);
                    }
                    else if (fe.selected == "2"){
                        // 组件设置测试
                        JavaScriptSerializer ser = new JavaScriptSerializer();
                        var atta = ser.Deserialize<Dictionary<string, object>>(pl.Attack);
                        atta["range_min"] = Convert.ToSingle(atta["range_min"]) + 4;
                        atta["range_max"] = Convert.ToSingle(atta["range_max"]) + 4;
                        pl.Attack = ser.Serialize(atta);
                        Console.WriteLine("玩家攻击力将+4");
						var acb = ser.Deserialize<Dictionary<string, object>>(pl.CollisionBox);
						acb["width"] = Convert.ToSingle(acb["width"]) + 1;
						acb["height"] = Convert.ToSingle(acb["height"]) + 1;
						//pl.CollisionBox = ser.Serialize(acb);
						//Console.WriteLine("玩家碰撞箱宽和高的值将+1格");
						var ahe = ser.Deserialize<Dictionary<string, object>>(pl.Health);
						ahe["max"] = Convert.ToSingle(ahe["max"]) + 10;
						ahe["value"] = Convert.ToSingle(ahe["value"]) + 10;
						pl.Health = ser.Serialize(ahe);
						Console.WriteLine("玩家当前和最大生命值将+10点");
						var prex = "[前缀]";
						var pname = pl.getName();
						bool alwaysshow = false;
						if (pname.IndexOf(prex) == 0) {
							pname = pname.Substring(prex.Length);
							alwaysshow = true;
						}
						else {
							pname = prex + pname;
							alwaysshow = false;
						}
						pl.setName(pname, alwaysshow);
						Console.WriteLine("玩家名字将添加/删除前缀，去掉/恢复常显");
						var apos = ser.Deserialize<Dictionary<string, object>>(pl.Position);
						apos["x"] = Convert.ToSingle(apos["x"]) + 16;
						apos["y"] = Convert.ToSingle(apos["y"]) + 10;
						apos["z"] = Convert.ToSingle(apos["z"]) + 16;
						//pl.Position = ser.Serialize(apos);
						//Console.WriteLine("玩家将位移至当前位置的(+16,+10,+16)上。");
						var arot = ser.Deserialize<Dictionary<string, object>>(pl.Rotation);
						arot["x"] = Convert.ToSingle(arot["x"]) + 16;
						arot["y"] = Convert.ToSingle(arot["y"]) + 16;
						pl.Rotation = ser.Serialize(arot);
						Console.WriteLine("玩家俯角+16，转角+16");
					}
					else if (fe.selected == "3")
                    {
						var el = CsActor.getsFromAABB(api, fe.dimensionid, fe.XYZ.x - 16, fe.XYZ.y - 16, fe.XYZ.z - 16,
							fe.XYZ.x + 16, fe.XYZ.y + 16, fe.XYZ.z + 16);
						if (el != null && el.Count > 0)
                        {
							Console.WriteLine("查询并移除玩家附近16格内所有实体：");
							foreach (IntPtr eptr in el)
                            {
								var cse = new CsActor(api, eptr);
								Console.WriteLine("TypeId={0},UniqueId={1},name={2}", cse.TypeId, cse.UniqueId, cse.getName());
								cse.remove();
                            }
                        }
                    } else if (fe.selected == "4")
                    {
						var el = CsActor.getsFromAABB(api, fe.dimensionid, fe.XYZ.x - 16, fe.XYZ.y - 16, fe.XYZ.z - 16,
							fe.XYZ.x + 16, fe.XYZ.y + 16, fe.XYZ.z + 16);
						var plst = CsPlayer.getplFromAABB(api, fe.dimensionid, fe.XYZ.x - 16, fe.XYZ.y - 16, fe.XYZ.z - 16,
							fe.XYZ.x + 16, fe.XYZ.y + 16, fe.XYZ.z + 16);
						el = el == null ? new ArrayList() : el;
						plst = plst == null ? new ArrayList() : plst;
						el.AddRange(plst);
						if (el.Count > 0)
						{
							Console.WriteLine("查询并模拟攻击玩家附近16格内所有实体和玩家：");
							foreach (IntPtr eptr in el)
							{
								var cse = new CsActor(api, eptr);
								Console.WriteLine("TypeId={0},UniqueId={1},name={2}", cse.TypeId, cse.UniqueId, cse.getName());
								// 测试实体模拟受攻击伤害，伤害值为10
								cse.hurt(fe.playerPtr/*IntPtr.Zero*/, ActorDamageCause.EntityAttack, 10, false, false);
							}
						}
					} else if (fe.selected == "5")
                    {
						pl.addLevel(3);
                    }
				}
                else{
                    Console.WriteLine("玩家 {0} 取消了表单 id={1}", fe.playername, fe.formid);
                }
                return false;
            });
			// 使用物品监听
			api.addAfterActListener(EventKey.onUseItem, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var ue = BaseEvent.getFrom(x) as UseItemEvent;
				if (ue != null && ue.RESULT) {
					Console.WriteLine("玩家 {0} 对 {1} 的 ({2}, {3}, {4}) 处的 {5} 方块" +
						"操作了 {6} 物品。", ue.playername, ue.dimension, ue.position.x, ue.position.y, ue.position.z, ue.blockname, ue.itemname);
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
			// 输入指令监听
			api.addBeforeActListener(EventKey.onInputCommand, x => {
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as InputCommandEvent;
				if (e != null) {
					Console.WriteLine(" <{0}> {1}", e.playername, e.cmd);
				}
				return true;
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
			// 玩家切换装备监听
			api.addAfterActListener(EventKey.onEquippedArmor, x =>
			{
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as EquippedArmorEvent;
				if (e != null)
				{
					Console.WriteLine("玩家 {0} 已于 {1} 的 ({2:F2},{3:F2},{4:F2}) 处切换 {5} 第 {6} 格的装备为 {7} 。",
						e.playername, e.dimension, e.XYZ.x, e.XYZ.y, e.XYZ.z,
						e.slottype == 0?"身体装备的":"主副手的", e.slot, e.itemname);
				}
				return true;
			});
			// 玩家升级监听
			api.addAfterActListener(EventKey.onLevelUp, x =>
			{
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as LevelUpEvent;
				if (e != null)
				{
					Console.WriteLine("玩家 {0} 已于 {1} 的 ({2:F2},{3:F2},{4:F2}) 处等级提升了 {5} 级。",
						e.playername, e.dimension, e.XYZ.x, e.XYZ.y, e.XYZ.z,
						e.lv);
				}
				return true;
			});
			// 活塞推方块监听
			api.addBeforeActListener(EventKey.onPistonPush, x =>
			{
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as PistonPushEvent;
				if (e != null)
				{
					Console.WriteLine("活塞 {0} 于 {1} 的({2}, {3}, {4})处试图向 {5} 号方向推拽({6}, {7}, {8})处的 {9} 方块。",
						e.blockname, e.dimension, e.position.x, e.position.y, e.position.z,
						e.direction, e.targetposition.x, e.targetposition.y, e.position.z, e.targetblockname);
					return true;
				}
				return true;
			});
			// 箱子合并监听
			api.addAfterActListener(EventKey.onChestPair, x =>
			{
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as ChestPairEvent;
				if (e != null)
				{
					Console.WriteLine("箱子 {0} 于 {1} 的({2}, {3}, {4})处试图合并向({5}, {6}, {7})处的 {8} 箱子。",
						e.blockname, e.dimension, e.position.x, e.position.y, e.position.z, e.targetposition.x, e.targetposition.y,
						e.position.z, e.targetblockname);
					return true;
				}
				return true;
			});
			/*
			api.addBeforeActListener(EventKey.onMobSpawnCheck, x =>
			{
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				var e = BaseEvent.getFrom(x) as MobSpawnCheckEvent;
				if (e != null)
				{
					Console.WriteLine("生物 {0} 于 {1} 的({2}, {3}, {4})处试图检查生成规则。",
						e.mobtype, e.dimension, e.XYZ.x, e.XYZ.y, e.XYZ.z);
					return true;
				}
				return true;
			});
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
					CsPlayer p = new CsPlayer(api, ue.playerPtr);
					Console.WriteLine("玩家 {0} 加入了游戏，xuid={1}, IP={2}", ue.playername, ue.xuid, p.IpPort);
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
					// 社区api测试
					api.setServerMotd(ae.playername + "发动了攻击", true);
					//Console.WriteLine("list={0}", api.getOnLinePlayers())
					CsPlayer p = new CsPlayer(api, ae.playerPtr);
					var uuid = p.Uuid;
					if (uuid != null)
					{
						tformid = api.sendSimpleForm((string)uuid,
											   "测试选项",
											   "test choose:",
											   "[\"基本组件\",\"物品栏组件\",\"组件设置\", \"范围检测并清理\",\"范围检测并攻击\"," +
											   "\"玩家等级+3\" ]");
						Console.WriteLine("创建需自行保管的表单，id={0}", tformid);
						// 非社区内容测试
						if (api.COMMERCIAL)
                        {
							CsActor ac = new CsActor(api, ae.attackedentityPtr);
							Console.WriteLine("目标实体的能力值：" + ac.Abilities + "\n目标实体的属性列表：" + ac.Attributes
								+ "\n目标实体的最大属性列表：" + ac.MaxAttributes + "\n目标实体的所有状态效果列表：" + ac.Effects);
                        }
					}
				} else {
					Console.WriteLine("Event convent fail.");
				}
				return true;
			});

			//玩家捡物品事件 - 可拦截
			api.addBeforeActListener(EventKey.onPickUpItem, x =>
			{
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				PickUpItemEvent ae = BaseEvent.getFrom(x) as PickUpItemEvent;
				if (ae != null)
				{
					Console.WriteLine("玩家 {0} 在 {1} 的 ({2}) 捡起了 {3} 物品。", ae.playername, ae.dimension, ae.XYZ.x.ToString("F2") + "," +
						ae.XYZ.y.ToString("F2") + "," + ae.XYZ.z.ToString("F2"), ae.itemname);
				}
				return true;
			});

			//玩家丢物品事件 - 可拦截
			api.addBeforeActListener(EventKey.onDropItem, x =>
			{
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				DropItemEvent ae = BaseEvent.getFrom(x) as DropItemEvent;
				if (ae != null)
				{
					Console.WriteLine("玩家 {0} 在 {1} 的 ({2}) 丢下了 {3} 物品。", ae.playername, ae.dimension, ae.XYZ.x.ToString("F2") + "," +
						ae.XYZ.y.ToString("F2") + "," + ae.XYZ.z.ToString("F2"), ae.itemname);
				}
				return true;
			});
			api.addAfterActListener(EventKey.onScoreChanged, x =>
			{
				Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
				ScoreChangedEvent ae = BaseEvent.getFrom(x) as ScoreChangedEvent;
				if (ae != null)
                {
					Console.WriteLine("计分板 {0} (显示名称：{1}，id：{2})分数改变为 {3}",
						ae.objectivename, ae.displayname, ae.scoreboardid, ae.score);
                }
				return true;
			});
			#region 非社区部分内容
			if (api.COMMERCIAL)
			{
				// 生物伤害监听
				api.addBeforeActListener(EventKey.onMobHurt, x => {
					Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
					var e = BaseEvent.getFrom(x) as MobHurtEvent;
					if (e != null && !string.IsNullOrEmpty(e.mobname))
					{
						Console.WriteLine(" {0} 在 {1} ({2:F2},{3:F2},{4:F2}) 即将受到来自 {5} 的 {6} 点伤害，类型 {7}",
							e.mobname, e.dimension, e.XYZ.x, e.XYZ.y, e.XYZ.z, e.srcname, e.dmcount, e.dmtype);
					}
					return true;
				});
				// 命令块执行指令监听，拦截
				api.addBeforeActListener(EventKey.onBlockCmd, x => {
					Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
					var e = BaseEvent.getFrom(x) as BlockCmdEvent;
					if (e != null)
					{
						Console.WriteLine("位于 {0} ({1},{2},{3}) 的 {4} 试图执行指令 {5}",
							e.dimension, e.position.x, e.position.y, e.position.z, e.name, e.cmd);
					}
					return false;
				});
				// NPC执行指令监听，拦截
				api.addBeforeActListener(EventKey.onNpcCmd, x => {
					Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
					var e = BaseEvent.getFrom(x) as NpcCmdEvent;
					if (e != null)
					{
						Console.WriteLine("位于 {0} ({1},{2},{3}) 的 {4} 试图执行第 {5} 条指令，指令集\n{6}",
							e.dimension, e.position.x, e.position.y, e.position.z, e.npcname, e.actionid, e.actions);
					}
					return false;
				});
				// 更新命令方块监听
				api.addBeforeActListener(EventKey.onCommandBlockUpdate, x => {
					Console.WriteLine("[CS] type = {0}, mode = {1}, result= {2}", x.type, x.mode, x.result);
					var e = BaseEvent.getFrom(x) as CommandBlockUpdateEvent;
					if (e != null)
					{
						Console.WriteLine(" {0} 试图修改位于 {1} ({2},{3},{4}) 的 {5} 的命令为 {6}",
							e.playername, e.dimension, e.position.x, e.position.y, e.position.z,
							e.isblock ? "命令块" : "命令矿车", e.cmd);
					}
					return true;
				});
			}
			#endregion


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
			Console.WriteLine("[Demo] CSR测试插件已装载。");
		}
	}
}