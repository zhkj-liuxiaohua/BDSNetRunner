/*
 * 由SharpDevelop创建。
 * 用户： BDSNetRunner
 * 日期: 2020/7/19
 * 时间: 16:36
 * 
 * 要改变这种模板请点击 工具|选项|代码编写|编辑标准头文件
 */
using System;
using System.Runtime.InteropServices;
using System.Text;

namespace CSR
{
	public static class EventKey {
		/// <summary>
		/// 无作用
		/// </summary>
		public const string Nothing = "Nothing";
		/// <summary>
		/// onServerCmd - 后台指令监听
		/// </summary>
		public const string onServerCmd = "onServerCmd";
		/// <summary>
		/// onServerCmdOutput - 后台指令输出信息监听
		/// </summary>
		public const string onServerCmdOutput = "onServerCmdOutput";
		/// <summary>
		/// onFormSelect - 玩家选择GUI表单监听
		/// </summary>
		public const string onFormSelect = "onFormSelect";
		/// <summary>
		/// onUseItem - 使用物品监听
		/// </summary>
		public const string onUseItem = "onUseItem";
		/// <summary>
		/// onPlacedBlock - 放置方块监听
		/// </summary>
		public const string onPlacedBlock = "onPlacedBlock";
		/// <summary>
		/// onDestroyBlock - 破坏方块监听
		/// </summary>
		public const string onDestroyBlock = "onDestroyBlock";
		/// <summary>
		/// onStartOpenChest - 开箱监听
		/// </summary>
		public const string onStartOpenChest = "onStartOpenChest";
		/// <summary>
		/// onStartOpenBarrel - 开桶监听
		/// </summary>
		public const string onStartOpenBarrel = "onStartOpenBarrel";
		/// <summary>
		/// onStopOpenChest - 关箱监听
		/// </summary>
		public const string onStopOpenChest = "onStopOpenChest";
		/// <summary>
		/// onStopOpenBarrel - 关桶监听
		/// </summary>
		public const string onStopOpenBarrel = "onStopOpenBarrel";
		/// <summary>
		/// onSetSlot - 放入取出物品监听
		/// </summary>
		public const string onSetSlot = "onSetSlot";
		/// <summary>
		/// onChangeDimension - 切换维度监听
		/// </summary>
		public const string onChangeDimension = "onChangeDimension";
		/// <summary>
		/// onMobDie - 生物死亡监听
		/// </summary>
		public const string onMobDie = "onMobDie";
		/// <summary>
		/// onMobHurt - 生物受伤监听
		/// </summary>
		public const string onMobHurt = "onMobHurt";
		/// <summary>
		/// onRespawn - 玩家重生监听
		/// </summary>
		public const string onRespawn = "onRespawn";
		/// <summary>
		/// onChat - 聊天监听
		/// </summary>
		public const string onChat = "onChat";
		/// <summary>
		/// onInputText - 玩家输入文本监听
		/// </summary>
		public const string onInputText = "onInputText";
		/// <summary>
		/// onCommandBlockUpdate - 玩家更新命令方块监听
		/// </summary>
		public const string onCommandBlockUpdate = "onCommandBlockUpdate";
		/// <summary>
		/// onInputCommand - 玩家输入指令监听
		/// </summary>
		public const string onInputCommand = "onInputCommand";
		/// <summary>
		/// onBlockCmd - 命令方块(矿车)执行指令监听
		/// </summary>
		public const string onBlockCmd = "onBlockCmd";
		/// <summary>
		/// onNpcCmd - NPC执行指令监听
		/// </summary>
		public const string onNpcCmd = "onNpcCmd";
		/// <summary>
		/// onLoadName -  加载名字监听
		/// </summary>
		public const string onLoadName = "onLoadName";
		/// <summary>
		/// onPlayerLeft - 离开游戏监听
		/// </summary>
		public const string onPlayerLeft = "onPlayerLeft";
		/// <summary>
		/// onMove - 移动监听
		/// </summary>
		public const string onMove = "onMove";
		/// <summary>
		/// onAttack - 攻击监听
		/// </summary>
		public const string onAttack = "onAttack";
		/// <summary>
		/// onLevelExplode - 爆炸监听
		/// </summary>
		public const string onLevelExplode = "onLevelExplode";
	}
	
	public enum EventType {
		Nothing = 0,
		onServerCmd = 1,
		onServerCmdOutput = 2,
		onFormSelect = 3,
		onUseItem = 4,
		onPlacedBlock = 5,
		onDestroyBlock = 6,
		onStartOpenChest = 7,
		onStartOpenBarrel = 8,
		onStopOpenChest = 9,
		onStopOpenBarrel = 10,
		onSetSlot = 11,
		onChangeDimension = 12,
		onMobDie = 13,
		onMobHurt = 14,
		onRespawn = 15,
		onChat = 16,
		onInputText = 17,
		onCommandBlockUpdate = 18,
		onInputCommand = 19,
		onBlockCmd = 20,
		onNpcCmd = 21,
		onLoadName = 22,
		onPlayerLeft = 23,
		onMove = 24,
		onAttack = 25,
		onLevelExplode = 26
	}
	
	public enum ActMode {
		BEFORE = 0,
		AFTER = 1
	}
	
	[StructLayoutAttribute(LayoutKind.Sequential)]
	public struct Events {
		/// <summary>
		/// 事件类型
		/// </summary>
		public ushort type;
		/// <summary>
		/// 事件触发模式
		/// </summary>
		public ushort mode;
		/// <summary>
		/// 事件结果（注册After事件时，此值有效）
		/// </summary>
		public int result;
		/// <summary>
		/// 原始数据
		/// </summary>
		public IntPtr data;
	}
	
	public struct BPos3 {
		public int x, y, z;
	}
	public struct Vec3 {
		public float x, y, z;
	}
	
    public struct Std_String {
		public IntPtr data;
		public ulong sd;
		public ulong len;
		public ulong uk3;
	}


	public class StrTool {
		[DllImport("kernel32.dll", CharSet = CharSet.Ansi, ExactSpelling = true)]
		internal static extern int lstrlenA(IntPtr ptr);

		// 从目标位置获取utf8字符串
		public static string readUTF8str(IntPtr ptr)
		{
			if (IntPtr.Zero == ptr) {
				return null;
			}
			int l = lstrlenA(ptr);
			if (l == 0) {
				return string.Empty;
			}
			var b = new byte[l];
			Marshal.Copy(ptr, b, 0, l);
			return Encoding.UTF8.GetString(b);
		}
		// 四字节转浮点
		public static float itof(int x) {
			return BitConverter.ToSingle(BitConverter.GetBytes(x), 0);
		}
		// std::string中读取c_str
		public static string c_str(Std_String s)
        {
			try
            {
				if (s.len < 1)
					return String.Empty;
				if (s.len < 16)
				{
					byte[] c = BitConverter.GetBytes((ulong)s.data);
					byte[] d = BitConverter.GetBytes(s.sd);
					byte[] str = new byte[16];
					Array.Copy(c, str, 8);
					Array.Copy(d, 0, str, 8, 8);
					return Encoding.UTF8.GetString(str, 0, (int)s.len);
				}
				return readUTF8str(s.data);
			}
            catch (Exception e){ Console.WriteLine(e.StackTrace); }
			return null;
		}
	}
	
	public class BaseEvent {
		protected EventType mTYPE;
		protected ActMode mMODE;
		protected bool mRESULT;
		/// <summary>
		/// 事件类型
		/// </summary>
		public EventType TYPE {get{return mTYPE;}}
		/// <summary>
		/// 事件触发模式
		/// </summary>
		public ActMode MODE {get{return mMODE;}}
		/// <summary>
		/// 事件结果（注册After事件时，此值有效）
		/// </summary>
		public bool RESULT {get{return mRESULT;}}
		
		/// <summary>
		/// 解析一个事件数据
		/// </summary>
		/// <param name="e"></param>
		/// <returns></returns>
		public static BaseEvent getFrom(Events e)
		{
			if (e.data == IntPtr.Zero) {
				Console.WriteLine("Empty struct data.");
				return null;
			}
			try
			{
				switch ((EventType)e.type)
				{
					case EventType.onServerCmd:
						return ServerCmdEvent.getFrom(e);
					case EventType.onServerCmdOutput:
						return ServerCmdOutputEvent.getFrom(e);
					case EventType.onFormSelect:
						return FormSelectEvent.getFrom(e);
					case EventType.onUseItem:
						return UseItemEvent.getFrom(e);
					case EventType.onPlacedBlock:
						return PlacedBlockEvent.getFrom(e);
					case EventType.onDestroyBlock:
						return DestroyBlockEvent.getFrom(e);
					case EventType.onStartOpenChest:
						return StartOpenChestEvent.getFrom(e);
					case EventType.onStartOpenBarrel:
						return StartOpenBarrelEvent.getFrom(e);
					case EventType.onStopOpenChest:
						return StopOpenChestEvent.getFrom(e);
					case EventType.onStopOpenBarrel:
						return StopOpenBarrelEvent.getFrom(e);
					case EventType.onSetSlot:
						return SetSlotEvent.getFrom(e);
					case EventType.onChangeDimension:
						return ChangeDimensionEvent.getFrom(e);
					case EventType.onMobDie:
						return MobDieEvent.getFrom(e);
					case EventType.onMobHurt:
						return MobHurtEvent.getFrom(e);
					case EventType.onRespawn:
						return RespawnEvent.getFrom(e);
					case EventType.onChat:
						return ChatEvent.getFrom(e);
					case EventType.onInputText:
						return InputTextEvent.getFrom(e);
					case EventType.onCommandBlockUpdate:
						return CommandBlockUpdateEvent.getFrom(e);
					case EventType.onInputCommand:
						return InputCommandEvent.getFrom(e);
					case EventType.onBlockCmd:
						return BlockCmdEvent.getFrom(e);
					case EventType.onNpcCmd:
						return NpcCmdEvent.getFrom(e);
					case EventType.onLoadName:
						return LoadNameEvent.getFrom(e);
					case EventType.onPlayerLeft:
						return PlayerLeftEvent.getFrom(e);
					case EventType.onMove:
						return MoveEvent.getFrom(e);
					case EventType.onAttack:
						return AttackEvent.getFrom(e);
					case EventType.onLevelExplode:
						return LevelExplodeEvent.getFrom(e);
					default:
						// do nothing
						break;
				}
			} catch (Exception ex) { Console.WriteLine(ex.StackTrace); }
			return null;
		}
		
		/// <summary>
		/// 构造一个事件头部
		/// </summary>
		/// <param name="e">原始事件数据</param>
		/// <param name="et">事件类型</param>
		/// <param name="t">类类型</param>
		/// <returns></returns>
		protected static BaseEvent createHead(Events e, EventType et, Type t)
		{
			if ((EventType)e.type == et) {
				Object o = t.Assembly.CreateInstance(t.FullName);
				var be = o as BaseEvent;
				if (be != null) {
					be.mTYPE = (EventType)e.type;
					be.mMODE = (ActMode)e.mode;
					be.mRESULT = (e.result == 1);
					return be;
				}
			}
			else
				Console.WriteLine("Event type mismatch.");
			return null;
		}
	}

	/// <summary>
	/// 后台指令监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class ServerCmdEvent : BaseEvent {
		protected string mcmd;
		/// <summary>
		/// 指令数据
		/// </summary>
		public string cmd {get {return mcmd;}}
		public static new ServerCmdEvent getFrom(Events e)
		{
			var sce = createHead(e, EventType.onServerCmd, typeof(ServerCmdEvent)) as ServerCmdEvent;
			if (sce == null)
				return sce;
			IntPtr s = e.data;	// 此处为转换过程
			sce.mcmd = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 0));
			return sce;
		}
	}
	
	/// <summary>
	/// 后台指令输出信息监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class ServerCmdOutputEvent : BaseEvent {
		protected string moutput;
		/// <summary>
		/// 指令数据
		/// </summary>
		public string output {get{return moutput;}}
		public static new ServerCmdOutputEvent getFrom(Events e)
		{
			var soe = createHead(e, EventType.onServerCmdOutput, typeof(ServerCmdOutputEvent)) as ServerCmdOutputEvent;
			if (soe == null)
				return soe;
			IntPtr s = e.data;	// 此处为转换过程
			soe.moutput = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 0));
			return soe;
		}
	}
	
	public class PlayerEvent : BaseEvent {
		protected string mplayername;
		protected string mdimension;
		protected Vec3 mXYZ;
		protected int mdimensionid;
		protected bool misstand;
		/// <summary>
		/// 玩家名字
		/// </summary>
		public string playername {get{return mplayername;}}
		/// <summary>
		/// 玩家所在维度
		/// </summary>
		public string dimension {get{return mdimension;}}
		/// <summary>
		/// 玩家所处位置
		/// </summary>
		public Vec3 XYZ {get{return mXYZ;}}
		/// <summary>
		/// 玩家所在维度ID
		/// </summary>
		public int dimensionid {get{return mdimensionid;}}
		/// <summary>
		/// 玩家是否立足于方块/悬空
		/// </summary>
		public bool isstand {get{return misstand;}}
		protected void loadData(IntPtr s) {
			// 此处为转换过程
			mplayername = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 0));
			mdimension = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 8));
			mXYZ = (Vec3)Marshal.PtrToStructure(s+16, typeof(Vec3));
			mdimensionid = Marshal.ReadInt32(s, 28);
			misstand = Marshal.ReadByte(s, 32) == 1;
		}
	}
	
	/// <summary>
	/// 玩家选择GUI表单监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class FormSelectEvent : PlayerEvent {
		protected string muuid;
		protected string mselected;
		protected int mformid;
		/// <summary>
		/// 玩家uuid信息
		/// </summary>
		public string uuid {get{return muuid;}}
		/// <summary>
		/// 表单回传的选择项信息
		/// </summary>
		public string selected {get{return mselected;}}
		/// <summary>
		/// 表单ID
		/// </summary>
		public int formid {get{return mformid;}}
		public static new FormSelectEvent getFrom(Events e)
		{
			var fse = createHead(e, EventType.onFormSelect, typeof(FormSelectEvent)) as FormSelectEvent;
			if (fse == null)
				return fse;
			IntPtr s = e.data;	// 此处为转换过程
			fse.loadData(s);
			fse.muuid = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 40));
			fse.mselected = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 48));
			fse.mformid = Marshal.ReadInt32(s, 56);
			return fse;
		}
	}
	
	/// <summary>
	/// 使用物品监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class UseItemEvent : PlayerEvent {
		protected string mitemname;
		protected BPos3 mposition;
		protected short mitemid;
		protected short mitemaux;
		protected string mblockname;
		protected short mblockid;
		/// <summary>
		/// 物品名称
		/// </summary>
		public string itemname {get{return mitemname;}}
		/// <summary>
		/// 操作方块所在位置
		/// </summary>
		public BPos3 position {get{return mposition;}}
		/// <summary>
		/// 物品ID
		/// </summary>
		public short itemid {get{return mitemid;}}
		/// <summary>
		/// 物品特殊值
		/// </summary>
		public short itemaux {get{return mitemaux;}}
		/// <summary>
		/// 操作方块名称
		/// </summary>
		public string blockname { get { return mblockname; } }
		/// <summary>
		/// 操作方块id
		/// </summary>
		public short blockid { get { return mblockid; } }
		public static new UseItemEvent getFrom(Events e)
		{
			var ue = createHead(e, EventType.onUseItem, typeof(UseItemEvent)) as UseItemEvent;
			if (ue == null)
				return null;
			IntPtr s = e.data;	// 此处为转换过程
			ue.loadData(s);
			ue.mitemname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 40));
			ue.mposition = (BPos3)Marshal.PtrToStructure(s + 48, typeof(BPos3));
			ue.mitemid = Marshal.ReadInt16(s, 60);
			ue.mitemaux = Marshal.ReadInt16(s, 62);
			ue.mblockname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 64));
			ue.mblockid = Marshal.ReadInt16(s, 80);
			return ue;
		}
	}
	
	public class BlockEvent : PlayerEvent {
		protected string mblockname;
		protected BPos3 mposition;
		protected short mblockid;
		/// <summary>
		/// 方块名称
		/// </summary>
		public string blockname {get{return mblockname;}}
		/// <summary>
		/// 操作方块所在位置
		/// </summary>
		public BPos3 position {get{return mposition;}}
		/// <summary>
		/// 方块ID
		/// </summary>
		public short blockid {get{return mblockid;}}
		protected new void loadData(IntPtr s) {
			base.loadData(s);
			// 此处为转换过程
			mblockname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 40));
			mposition = (BPos3)Marshal.PtrToStructure(s + 48, typeof(BPos3));
			mblockid = Marshal.ReadInt16(s, 60);
		}
	}
	
	/// <summary>
	/// 放置方块监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class PlacedBlockEvent : BlockEvent {
		public static new PlacedBlockEvent getFrom(Events e)
		{
			var ple = createHead(e, EventType.onPlacedBlock, typeof(PlacedBlockEvent)) as PlacedBlockEvent;
			if (ple == null)
				return null;
			ple.loadData(e.data);
			return ple;
		}
	}
	
	/// <summary>
	/// 破坏方块监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class DestroyBlockEvent : BlockEvent {
		public static new DestroyBlockEvent getFrom(Events e)
		{
			var ple = createHead(e, EventType.onDestroyBlock, typeof(DestroyBlockEvent)) as DestroyBlockEvent;
			if (ple == null)
				return null;
			ple.loadData(e.data);
			return ple;
		}
	}
	
	/// <summary>
	/// 开箱监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class StartOpenChestEvent : BlockEvent {
		public static new StartOpenChestEvent getFrom(Events e)
		{
			var ple = createHead(e, EventType.onStartOpenChest, typeof(StartOpenChestEvent)) as StartOpenChestEvent;
			if (ple == null)
				return null;
			ple.loadData(e.data);
			return ple;
		}
	}
	
	/// <summary>
	/// 开桶监听<br/>
	/// 拦截可否：否
	/// </summary>
	public class StartOpenBarrelEvent : BlockEvent {
		public static new StartOpenBarrelEvent getFrom(Events e)
		{
			var ple = createHead(e, EventType.onStartOpenBarrel, typeof(StartOpenBarrelEvent)) as StartOpenBarrelEvent;
			if (ple == null)
				return null;
			ple.loadData(e.data);
			return ple;
		}
	}
	
	/// <summary>
	/// 关箱监听<br/>
	/// 拦截可否：否
	/// </summary>
	public class StopOpenChestEvent : BlockEvent {
		public static new StopOpenChestEvent getFrom(Events e)
		{
			var ple = createHead(e, EventType.onStopOpenChest, typeof(StopOpenChestEvent)) as StopOpenChestEvent;
			if (ple == null)
				return null;
			ple.loadData(e.data);
			return ple;
		}
	}
	
	/// <summary>
	/// 关桶监听<br/>
	/// 拦截可否：否
	/// </summary>
	public class StopOpenBarrelEvent : BlockEvent {
		public static new StopOpenBarrelEvent getFrom(Events e)
		{
			var ple = createHead(e, EventType.onStopOpenBarrel, typeof(StopOpenBarrelEvent)) as StopOpenBarrelEvent;
			if (ple == null)
				return null;
			ple.loadData(e.data);
			return ple;
		}
	}
	
	/// <summary>
	/// 放入取出物品监听<br/>
	/// 拦截可否：否
	/// </summary>
	public class SetSlotEvent : PlayerEvent {
		protected string mitemname;
		protected string mblockname;
		protected BPos3 mposition;
		protected int mitemcount;
		protected int mslot;
		protected short mitemaux;
		protected short mblockid;
		protected short mitemid;
		/// <summary>
		/// 物品名字
		/// </summary>
		public string itemname {get{return mitemname;}}
		/// <summary>
		/// 方块名称
		/// </summary>
		public string blockname {get{return mblockname;}}
		/// <summary>
		/// 操作方块所在位置
		/// </summary>
		public BPos3 position {get{return mposition;}}
		/// <summary>
		/// 物品数量
		/// </summary>
		public int itemcount {get{return mitemcount;}}
		/// <summary>
		/// 操作格子位置
		/// </summary>
		public int slot {get{return mslot;}}
		/// <summary>
		/// 物品特殊值
		/// </summary>
		public short itemaux {get{return mitemaux;}}
		/// <summary>
		/// 方块ID
		/// </summary>
		public short blockid {get{return mblockid;}}
		/// <summary>
		/// 物品ID
		/// </summary>
		public short itemid {get{return mitemid;}}
		public static new SetSlotEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onSetSlot, typeof(SetSlotEvent)) as SetSlotEvent;
			if (le == null)
				return null;
			IntPtr s = e.data;	// 此处为转换过程
			le.loadData(s);
			le.mitemname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 40));
			le.mblockname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 48));
			le.mposition = (BPos3)Marshal.PtrToStructure(s+56, typeof(BPos3));
			le.mitemcount = Marshal.ReadInt32(s, 68);
			le.mslot = Marshal.ReadInt32(s, 72);
			le.mitemaux = Marshal.ReadInt16(s, 76);
			le.mblockid = Marshal.ReadInt16(s, 78);
			le.mitemid = Marshal.ReadInt16(s, 80);
			return le;
		}
		
	}
	
	/// <summary>
	/// 切换维度监听<br/>
	/// 拦截可否：否
	/// </summary>
	public class ChangeDimensionEvent : PlayerEvent {
		public static new ChangeDimensionEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onChangeDimension, typeof(ChangeDimensionEvent)) as ChangeDimensionEvent;
			if (le == null)
				return null;
			le.loadData(e.data);
			return le;
		}
	}
	
	public class HurtEvent : BaseEvent {
		protected string mmobname;
		protected string mplayername;
		protected string mdimension;
		protected string mmobtype;
		protected string msrcname;
		protected string msrctype;
		protected Vec3 mXYZ;
		protected int mdimensionid;
		protected int mdmcase;
		protected bool misstand;
		/// <summary>
		/// 生物名称
		/// </summary>
		public string mobname {get{return mmobname;}}
		/// <summary>
		/// 玩家名字（若为玩家死亡，附加此信息）
		/// </summary>
		public string playername {get{return mplayername;}}
		/// <summary>
		/// 玩家所在维度（附加信息）
		/// </summary>
		public string dimension {get{return mdimension;}}
		/// <summary>
		/// 生物类型
		/// </summary>
		public string mobtype {get{return mmobtype;}}
		/// <summary>
		/// 伤害源名称
		/// </summary>
		public string srcname {get{return msrcname;}}
		/// <summary>
		/// 伤害源类型
		/// </summary>
		public string srctype {get{return msrctype;}}
		/// <summary>
		/// 生物所在位置
		/// </summary>
		public Vec3 XYZ {get{return mXYZ;}}
		/// <summary>
		/// 生物所处维度ID
		/// </summary>
		public int dimensionid {get{return mdimensionid;}}
		/// <summary>
		/// 伤害具体原因ID
		/// </summary>
		public int dmcase {get{return mdmcase;}}
		/// <summary>
		/// 玩家是否立足于方块/悬空（附加信息）
		/// </summary>
		public bool isstand {get{return misstand;}}
		protected void loadData(IntPtr s) {
			// 此处为转换过程
			mmobname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 0));
			mplayername = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 8));
			mdimension = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 16));
			mmobtype = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 24));
			msrcname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 32));
			msrctype = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 40));
			mXYZ = (Vec3)Marshal.PtrToStructure(s+48, typeof(Vec3));
			mdimensionid = Marshal.ReadInt32(s, 60);
			mdmcase = Marshal.ReadInt32(s, 64);
			misstand = Marshal.ReadByte(s, 68) == 1;
		}
	}
	
	/// <summary>
	/// 生物死亡监听<br/>
	/// 拦截可否：否
	/// </summary>
	public class MobDieEvent : HurtEvent {
		public static new MobDieEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onMobDie, typeof(MobDieEvent)) as MobDieEvent;
			if (le == null)
				return null;
			le.loadData(e.data);
			return le;
		}
	}
	
	/// <summary>
	/// 生物受伤监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class MobHurtEvent : HurtEvent {
		protected string mdmtype;
		protected float mhealth;
		protected int mdmcount;
		/// <summary>
		/// 生物受伤类型
		/// </summary>
		public string dmtype {get{return mdmtype;}}
		/// <summary>
		/// 生物血量
		/// </summary>
		public float health {get{return mhealth;}}
		/// <summary>
		/// 生物受伤具体值
		/// </summary>
		public int dmcount {get{return mdmcount;}}
		public static new MobHurtEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onMobHurt, typeof(MobHurtEvent)) as MobHurtEvent;
			if (le == null)
				return null;
			IntPtr s = e.data;
			le.loadData(s);
			le.mdmtype = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 72));
			le.mhealth = StrTool.itof(Marshal.ReadInt32(s, 80));
			le.mdmcount = Marshal.ReadInt32(s, 84);
			return le;
		}
	}
	
	/// <summary>
	/// 玩家重生监听<br/>
	/// 拦截可否：否
	/// </summary>
	public class RespawnEvent : PlayerEvent {
		public static new RespawnEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onRespawn, typeof(RespawnEvent)) as RespawnEvent;
			if (le == null)
				return null;
			le.loadData(e.data);
			return le;
		}
	}
	
	/// <summary>
	/// 聊天监听<br/>
	/// 拦截可否：否
	/// </summary>
	public class ChatEvent : BaseEvent {
		protected string mplayername;
		protected string mtarget;
		protected string mmsg;
		protected string mchatstyle;
		/// <summary>
		/// 发言人名字（可能为服务器或命令方块发言）
		/// </summary>
		public string playername {get{return mplayername;}}
		/// <summary>
		/// 接收者名字
		/// </summary>
		public string target {get{return mtarget;}}
		/// <summary>
		/// 接收到的信息
		/// </summary>
		public string msg {get{return mmsg;}}
		/// <summary>
		/// 聊天类型
		/// </summary>
		public string chatstyle {get{return mchatstyle;}}
		public static new ChatEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onChat, typeof(ChatEvent)) as ChatEvent;
			if (le == null)
				return null;
			IntPtr s = e.data;	// 此处为转换过程
			le.mplayername = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 0));
			le.mtarget = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 8));
			le.mmsg = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 16));
			le.mchatstyle = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 24));
			return le;
		}
	}
	
	/// <summary>
	/// 玩家输入文本监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class InputTextEvent : PlayerEvent {
		protected string mmsg;
		/// <summary>
		/// 输入的文本
		/// </summary>
		public string msg {get{return mmsg;}}
		public static new InputTextEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onInputText, typeof(InputTextEvent)) as InputTextEvent;
			if (le == null)
				return null;
			IntPtr s = e.data;
			le.loadData(s);
			le.mmsg = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 40));
			return le;
		}
	}
	
	/// <summary>
	/// 玩家更新命令方块监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class CommandBlockUpdateEvent : PlayerEvent {
		protected string mcmd;
		protected string mactortype;
		protected BPos3 mposition;
		protected bool misblock;
		/// <summary>
		/// 将被更新的新指令
		/// </summary>
		public string cmd {get{return mcmd;}}
		/// <summary>
		/// 实体类型（若被更新的是非方块，附加此信息）
		/// </summary>
		public string actortype {get{return mactortype;}}
		/// <summary>
		/// 命令方块所在位置
		/// </summary>
		public BPos3 position {get{return mposition;}}
		/// <summary>
		/// 是否是方块
		/// </summary>
		public bool isblock {get{return misblock;}}
		public static new CommandBlockUpdateEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onCommandBlockUpdate, typeof(CommandBlockUpdateEvent)) as CommandBlockUpdateEvent;
			if (le == null)
				return null;
			IntPtr s = e.data;
			le.loadData(s);
			le.mcmd = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 40));
			le.mactortype = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 48));
			le.mposition = (BPos3)Marshal.PtrToStructure(s+56, typeof(BPos3));
			le.misblock = Marshal.ReadByte(s, 68) == 1;
			return le;
		}
	}

	/// <summary>
	/// 玩家输入指令监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class InputCommandEvent : PlayerEvent {
		protected string mcmd;
		/// <summary>
		/// 玩家输入的指令
		/// </summary>
		public string cmd {get{return mcmd;}}
		public static new InputCommandEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onInputCommand, typeof(InputCommandEvent)) as InputCommandEvent;
			if (le == null)
				return null;
			IntPtr s = e.data;
			le.loadData(s);
			le.mcmd = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 40));
			return le;
		}
	}
	
	/// <summary>
	/// 命令方块(矿车)执行指令监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class BlockCmdEvent : BaseEvent {
		protected string mcmd;
		protected string mname;
		protected string mdimension;
		protected BPos3 mposition;
		protected int mdimensionid;
		protected int mtickdelay;
		protected int mtype;
		/// <summary>
		/// 将被执行的指令
		/// </summary>
		public string cmd {get{return mcmd;}}
		/// <summary>
		/// 执行者自定义名称
		/// </summary>
		public string name {get{return mname;}}
		/// <summary>
		/// 命令块所处维度
		/// </summary>
		public string dimension {get{return mdimension;}}
		/// <summary>
		/// 执行者所在位置
		/// </summary>
		public BPos3 position {get{return mposition;}}
		/// <summary>
		/// 命令块所处维度ID
		/// </summary>
		public int dimensionid {get{return mdimensionid;}}
		/// <summary>
		/// 命令设定的间隔时间
		/// </summary>
		public int tickdelay {get{return mtickdelay;}}
		/// <summary>
		/// 执行者类型
		/// </summary>
		public int type {get{return mtype;}}
		public static new BlockCmdEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onBlockCmd, typeof(BlockCmdEvent)) as BlockCmdEvent;
			if (le == null)
				return null;
			IntPtr s = e.data; // 此处为转换过程
			le.mcmd = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 0));
			le.mname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 8));
			le.mdimension = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 16));
			le.mposition = (BPos3)Marshal.PtrToStructure(s+24, typeof(BPos3));
			le.mdimensionid = Marshal.ReadInt32(s, 36);
			le.mtickdelay = Marshal.ReadInt32(s, 40);
			le.mtype = Marshal.ReadInt32(s, 44);
			return le;
		}
	}
	
	public class NpcCmdEvent : BaseEvent {
		protected string mnpcname;
		protected string mentity;
		protected string mdimension;
		protected string mactions;
		protected Vec3 mposition;
		protected int mactionid;
		protected int mentityid;
		protected int mdimensionid;
		/// <summary>
		/// NPC名字
		/// </summary>
		public string npcname {get{return mnpcname;}}
		/// <summary>
		/// NPC实体标识名
		/// </summary>
		public string entity {get{return mentity;}}
		/// <summary>
		/// NPC所处维度
		/// </summary>
		public string dimension {get{return mdimension;}}
		/// <summary>
		/// 指令列表
		/// </summary>
		public string actions {get{return mactions;}}
		/// <summary>
		/// NPC所在位置
		/// </summary>
		public Vec3 position {get{return mposition;}}
		/// <summary>
		/// 选择项
		/// </summary>
		public int actionid {get{return mactionid;}}
		/// <summary>
		/// NPC实体标识ID
		/// </summary>
		public int entityid {get{return mentityid;}}
		/// <summary>
		/// NPC所处维度ID
		/// </summary>
		public int dimensionid {get{return mdimensionid;}}
		public static new NpcCmdEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onNpcCmd, typeof(NpcCmdEvent)) as NpcCmdEvent;
			if (le == null)
				return null;
			IntPtr s = e.data; // 此处为转换过程
			le.mnpcname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 0));
			le.mentity = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 8));
			le.mdimension = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 16));
			le.mactions = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 24));
			le.mposition = (Vec3)Marshal.PtrToStructure(s+32, typeof(Vec3));
			le.mactionid = Marshal.ReadInt32(s, 44);
			le.mentityid = Marshal.ReadInt32(s, 48);
			le.mdimensionid = Marshal.ReadInt32(s, 52);
			return le;
		}
	}
	
	/// <summary>
	/// 加载名字监听<br/>
	/// 拦截可否：否
	/// </summary>
	public class LoadNameEvent : BaseEvent {
		protected string mplayername;
		protected string muuid;
		protected string mxuid;
		protected string mability;
		/// <summary>
		/// 玩家名字
		/// </summary>
		public string playername {get{return mplayername;}}
		/// <summary>
		/// 玩家uuid字符串
		/// </summary>
		public string uuid {get{return muuid;}}
		/// <summary>
		/// 玩家xuid字符串
		/// </summary>
		public string xuid {get{return mxuid;}}
		/// <summary>
		/// 玩家能力值列表（可选，商业版可用）
		/// </summary>
		public string ability {get{return mability;}}
		public static new LoadNameEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onLoadName, typeof(LoadNameEvent)) as LoadNameEvent;
			if (le == null)
				return null;
			IntPtr s = e.data; // 此处为转换过程
			le.mplayername = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 0));
			le.muuid = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 8));
			le.mxuid = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 16));
			le.mability = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 24));
			return le;
		}
	}
	
	/// <summary>
	/// 离开游戏监听<br/>
	/// 拦截可否：否
	/// </summary>
	public class PlayerLeftEvent : LoadNameEvent {
		public static new PlayerLeftEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onPlayerLeft, typeof(PlayerLeftEvent)) as PlayerLeftEvent;
			if (le == null)
				return null;
			IntPtr s = e.data;	// 此处为转换过程
			le.mplayername = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 0));
			le.muuid = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 8));
			le.mxuid = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 16));
			le.mability = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 24));
			return le;
		}
	}
	
	public class MoveEvent : PlayerEvent {
		public static new MoveEvent getFrom(Events e)
		{
			var ate = createHead(e, EventType.onMove, typeof(MoveEvent)) as MoveEvent;
			if (ate == null)
				return null;
			ate.loadData(e.data);
			return ate;
		}
	}
	
	/// <summary>
	/// 玩家攻击监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class AttackEvent : PlayerEvent {
		protected string mactorname;
		protected string mactortype;
		protected Vec3 mactorpos;
		/// <summary>
		/// 被攻击实体名称
		/// </summary>
		public string actorname {get{return mactorname;}}
		/// <summary>
		/// 被攻击实体类型
		/// </summary>
		public string actortype {get{return mactortype;}}
		/// <summary>
		/// 被攻击实体所处位置
		/// </summary>
		public Vec3 actorpos {get{return mactorpos;}}
		public static new AttackEvent getFrom(Events e)
		{
			var ate = createHead(e, EventType.onAttack, typeof(AttackEvent)) as AttackEvent;
			if (ate == null)
				return null;
			IntPtr s = e.data;	// 此处为转换过程
			ate.loadData(s);
			ate.mactorname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 40));
			ate.mactortype = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 48));
			ate.mactorpos = (Vec3)Marshal.PtrToStructure(s + 56, typeof(Vec3));
			return ate;
		}
	}
		
	/// <summary>
	/// 世界范围爆炸监听<br/>
	/// 拦截可否：是
	/// </summary>
	public class LevelExplodeEvent : BaseEvent {
		protected string mentity;
		protected string mblockname;
		protected string mdimension;
		protected Vec3 mposition;
		protected int mentityid;
		protected int mdimensionid;
		protected float mexplodepower;
		protected short mblockid;
		/// <summary>
		/// 爆炸者实体标识名（可能为空）
		/// </summary>
		public string entity {get{return mentity;}}
		/// <summary>
		/// 爆炸方块名（可能为空）
		/// </summary>
		public string blockname {get{return mblockname;}}
		/// <summary>
		/// 爆炸者所处维度
		/// </summary>
		public string dimension {get{return mdimension;}}
		/// <summary>
		/// 爆炸点所在位置
		/// </summary>
		public Vec3 position {get{return mposition;}}
		/// <summary>
		/// 爆炸者实体标识ID
		/// </summary>
		public int entityid {get{return mentityid;}}
		/// <summary>
		/// 爆炸者所处维度ID
		/// </summary>
		public int dimensionid {get{return mdimensionid;}}
		/// <summary>
		/// 爆炸强度
		/// </summary>
		public float explodepower {get{return mexplodepower;}}
		/// <summary>
		/// 爆炸方块ID
		/// </summary>
		public float blockid {get{return mblockid;}}
		public static new LevelExplodeEvent getFrom(Events e)
		{
			var le = createHead(e, EventType.onLevelExplode, typeof(LevelExplodeEvent)) as LevelExplodeEvent;
			if (le == null)
				return null;
			IntPtr s = e.data;	// 此处为转换过程
			le.mentity = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 0));
			le.mblockname = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 8));
			le.mdimension = StrTool.readUTF8str((IntPtr)Marshal.ReadInt64(s, 16));
			le.mposition = (Vec3)Marshal.PtrToStructure(s + 24, typeof(Vec3));
			le.mentityid = Marshal.ReadInt32(s, 36);
			le.mdimensionid = Marshal.ReadInt32(s, 40);
			le.mexplodepower = StrTool.itof(Marshal.ReadInt32(s, 44));
			le.mblockid = Marshal.ReadInt16(s, 48);
			return le;
		}
	}
}
