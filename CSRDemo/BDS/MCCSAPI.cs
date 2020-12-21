/*
 * 由SharpDevelop创建。
 * 用户： BDSNetRunner
 * 日期: 2020/7/17
 * 时间: 16:27
 * 
 * 要改变这种模板请点击 工具|选项|代码编写|编辑标准头文件
 */
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace CSR
{
	/// <summary>
	/// API接口定义
	/// </summary>
	public class MCCSAPI
	{
		[DllImport("kernel32.dll")]
        private extern static IntPtr LoadLibrary(String path);
        [DllImport("kernel32.dll")]
        private extern static IntPtr GetProcAddress(IntPtr lib, String funcName);
        [DllImport("kernel32.dll")]
        private extern static bool FreeLibrary(IntPtr lib);

        private readonly string mVersion;
        /// <summary>
        /// 插件版本
        /// </summary>
        public string VERSION {get{return mVersion;}}
        private readonly bool mcommercial;
        /// <summary>
        /// 平台类型
        /// </summary>
        public bool COMMERCIAL {get{return mcommercial;}}

		// 注册事件回调管理，避免回收
		private Dictionary<string, ArrayList> callbks = new Dictionary<string, ArrayList>();

		private IntPtr hLib;
        public MCCSAPI(String DLLPath, string ver, bool commercial)
        {
        	mVersion = ver;
        	mcommercial = commercial;
            hLib = LoadLibrary(DLLPath);
            if (hLib != IntPtr.Zero) {
            	initApis();
            }
        }
        ~MCCSAPI()
        {
            FreeLibrary(hLib);
        }
        //将要执行的函数转换为委托
        private T Invoke<T>(String APIName) where T : Delegate
        {
            IntPtr api = GetProcAddress(hLib, APIName);
            if (api != IntPtr.Zero)
	            return (T)Marshal.GetDelegateForFunctionPointer(api, typeof(T));
				//若.net framework版本高于4.5.1可用以下替换以上
				//return Marshal.GetDelegateForFunctionPointer<T>(api);
            Console.WriteLine("Get Api {0} failed.", APIName);
            return null;
        }
        
        /// <summary>
        /// 事件处理函数类型
        /// </summary>
        /// <param name="e">原始数据</param>
        /// <returns>是否继续/拦截(before事件有效)</returns>
        public delegate bool EventCab(Events e);
        private delegate bool ADDACTEVENTFUNC(string key, EventCab cb);
        private ADDACTEVENTFUNC caddBeforeActEvent, caddAfterActEvent, cremoveBeforeAct, cremoveAfterAct;
		private delegate bool CSHOOKFUNC(int rva, IntPtr hook, out IntPtr org);
		private CSHOOKFUNC ccshook;
		private delegate bool CSUNHOOKFUNC(IntPtr hook, out IntPtr org);
		private CSUNHOOKFUNC ccsunhook;
		private delegate IntPtr DLSYMFUNC(int rva);
		private DLSYMFUNC cdlsym;
		private delegate bool READHARDMEMORY(int rva, byte[] odata, int size);
		private READHARDMEMORY creadHardMemory, cwriteHardMemory;
		private delegate void SETSHAREPTRFUNC(string key, IntPtr sdata);
		private SETSHAREPTRFUNC csetshareptr;
		private delegate IntPtr GETSHAREPTRFUNC(string key);
		private GETSHAREPTRFUNC cgetSharePtr, cremoveSharePtr;
		public enum CommandPermissionLevel {
			Any = 0,
			GameMasters = 1,
			Admin = 2,
			Host = 3,
			Owner = 4,
			Internal = 5
		}
		public enum CommandVisibilityFlag : byte {
			Visible = 0,
			HiddenFromCommandBlockOrigin = 2,
			HiddenFromPlayerOrigin = 4,
			Hidden = 6
		}
		public enum CommandUsageFlag : byte {
			Normal = 0,
			Test = 1
		}
		public enum CommandSyncFlag : byte {
			Synced = 0,
			Local = 8
		}
		public enum CommandExecuteFlag : byte {
			Allowed = 0,
			Disallowed = 0x10
		}
		public enum CommandTypeFlag : byte {
			None = 0,
			Message = 0x20
		}
		public enum CommandCheatFlag : byte {
			Cheat = 0,
			NotCheat = 0x40
		}
		private delegate void SETCOMMANDDESCRIBEFUNC(string key, string description, CommandPermissionLevel level, byte flag1, byte flag2);
		private SETCOMMANDDESCRIBEFUNC csetCommandDescribe;
		private delegate bool RUNCMDFUNC(string cmd);
		private RUNCMDFUNC cruncmd, cremovePlayerBossBar, cremovePlayerSidebar;
		private delegate void LOGOUTFUNC(string cmdout);
		private LOGOUTFUNC clogout;
		private delegate Std_String GETONLINEPLAYERSFUNC();
		private GETONLINEPLAYERSFUNC cgetOnLinePlayers;
		private delegate Std_String GETSTRUCTUREFUNC(int did, string jsonposa, string jsonposb, bool exent, bool exblk);
		private GETSTRUCTUREFUNC cgetStructure;
		private delegate bool SETSTRUCTUREFUNC(string jdata, int did, string jsonposa, byte rot, bool exent, bool exblk);
		private SETSTRUCTUREFUNC csetStructure;
		private delegate bool RENAMEBYUUIDFUNC(string uuid, string newName);
		private RENAMEBYUUIDFUNC creNameByUuid, csetPlayerAbilities, csetPlayerTempAttributes,
			csetPlayerMaxAttributes, csetPlayerItems, caddPlayerItemEx, csetPlayerEffects,
			ctalkAs, cruncmdAs, cdisconnectClient, csendText, csetPlayerPermissionAndGametype;
		private delegate Std_String GETPLAYERABILITIESFUNC(string uuid);
		private GETPLAYERABILITIESFUNC cgetPlayerAbilities, cgetPlayerAttributes, cgetPlayerMaxAttributes,
			cgetPlayerItems, cgetPlayerSelectedItem, cgetPlayerEffects, cselectPlayer, cgetPlayerPermissionAndGametype;
		private delegate bool ADDPLAYERITEMFUNC(string uuid, int id, short aux, byte count);
		private ADDPLAYERITEMFUNC caddPlayerItem;
		private delegate bool SETPLAYERBOSSBARFUNC(string uuid, string title, float percent);
		private SETPLAYERBOSSBARFUNC csetPlayerBossBar;
		private delegate bool TRANSFERSERVERFUNC(string uuid, string addr, int port);
		private TRANSFERSERVERFUNC ctransferserver;
		private delegate bool TELEPORTFUNC(string uuid, float x, float y, float z, int did);
		private TELEPORTFUNC cteleport;
		private delegate uint SENDSIMPLEFORMFUNC(string uuid, string title, string content, string buttons);
		private SENDSIMPLEFORMFUNC csendSimpleForm;
		private delegate uint SENDMODALFORMFUNC(string uuid, string title, string content, string button1, string button2);
		private SENDMODALFORMFUNC csendModalForm;
		private delegate uint SENDCUSTOMFORMFUNC(string uuid, string json);
		private SENDCUSTOMFORMFUNC csendCustomForm;
		private delegate bool RELEASEFORMFUNC(uint fid);
		private RELEASEFORMFUNC creleaseForm;
		private delegate bool SETPLAYERSIDEBARFUNC(string uuid, string title, string list);
		private SETPLAYERSIDEBARFUNC csetPlayerSidebar;
		private delegate int GETSCOREBOARDVALUEFUNC(string uuid, string objname);
		private GETSCOREBOARDVALUEFUNC cgetscoreboardValue;
		private delegate bool SETSCOREBOARDVALUEFUNC(string uuid, string objname, int count);
		private SETSCOREBOARDVALUEFUNC csetscoreboardValue;
		private delegate bool SETSERVERMOTD(string motd, bool isShow);
		private SETSERVERMOTD csetServerMotd;
		private delegate IntPtr GETEXTRAAPI(string apiname);
		private GETEXTRAAPI cgetExtraAPI;

		// 转换附加函数指针
		private T ConvertExtraFunc<T>(string apiname)  where T : Delegate
		{
			if (cgetExtraAPI != null) {
				IntPtr f = cgetExtraAPI(apiname);
				if (f != IntPtr.Zero) {
					return (T)Marshal.GetDelegateForFunctionPointer(f, typeof(T));
					//若.net framework版本高于4.5.1可用以下替换以上
					//return Marshal.GetDelegateForFunctionPointer<T>(f);
				}
			}
			Console.WriteLine("Get ExtraApi {0} failed.", apiname);
            return null;
        }

		private delegate IntPtr MCCOMPONENTAPI(string apiname);
		private MCCOMPONENTAPI cmcComponentAPI;
		// 获取组件相关API
		public T ConvertComponentFunc<T>(string apiname) where T : Delegate
        {
			if (cmcComponentAPI != null)
			{
				IntPtr f = cmcComponentAPI(apiname);
				if (f != IntPtr.Zero)
				{
					return (T)Marshal.GetDelegateForFunctionPointer(f, typeof(T));
					//若.net framework版本高于4.5.1可用以下替换以上
					//return Marshal.GetDelegateForFunctionPointer<T>(f);
				}
			}
			Console.WriteLine("Get ComponentApi {0} failed.", apiname);
			return null;
		}

		// 初始化所有api函数
		void initApis()
		{
			caddBeforeActEvent = Invoke<ADDACTEVENTFUNC>("addBeforeActListener");
			caddAfterActEvent = Invoke<ADDACTEVENTFUNC>("addAfterActListener");
			cremoveBeforeAct = Invoke<ADDACTEVENTFUNC>("removeBeforeActListener");
			cremoveAfterAct = Invoke<ADDACTEVENTFUNC>("removeAfterActListener");
			csetshareptr = Invoke<SETSHAREPTRFUNC>("setSharePtr");
			cgetSharePtr = Invoke<GETSHAREPTRFUNC>("getSharePtr");
			cremoveSharePtr = Invoke<GETSHAREPTRFUNC>("removeSharePtr");
			csetCommandDescribe = Invoke<SETCOMMANDDESCRIBEFUNC>("setCommandDescribeEx");
			cruncmd = Invoke<RUNCMDFUNC>("runcmd");
			clogout = Invoke<LOGOUTFUNC>("logout");
			cgetOnLinePlayers = Invoke<GETONLINEPLAYERSFUNC>("getOnLinePlayers");
			cgetExtraAPI = Invoke<GETEXTRAAPI>("getExtraAPI");
			cmcComponentAPI = Invoke<MCCOMPONENTAPI>("mcComponentAPI");
			creNameByUuid = Invoke<RENAMEBYUUIDFUNC>("reNameByUuid");
			ctalkAs = Invoke<RENAMEBYUUIDFUNC>("talkAs");
			cruncmdAs = Invoke<RENAMEBYUUIDFUNC>("runcmdAs");
			cdisconnectClient = Invoke<RENAMEBYUUIDFUNC>("disconnectClient");
			csendText = Invoke<RENAMEBYUUIDFUNC>("sendText");
			csendSimpleForm = Invoke<SENDSIMPLEFORMFUNC>("sendSimpleForm");
			csendModalForm = Invoke<SENDMODALFORMFUNC>("sendModalForm");
			csendCustomForm = Invoke<SENDCUSTOMFORMFUNC>("sendCustomForm");
			creleaseForm = Invoke<RELEASEFORMFUNC>("releaseForm");
			cselectPlayer = Invoke<GETPLAYERABILITIESFUNC>("selectPlayer");
			caddPlayerItem = Invoke<ADDPLAYERITEMFUNC>("addPlayerItem");
			cgetscoreboardValue = Invoke<GETSCOREBOARDVALUEFUNC>("getscoreboardValue");
			csetscoreboardValue = Invoke<SETSCOREBOARDVALUEFUNC>("setscoreboardValue");
			csetServerMotd = Invoke<SETSERVERMOTD>("setServerMotd");
			ccshook = Invoke<CSHOOKFUNC>("cshook");
			ccsunhook = Invoke<CSUNHOOKFUNC>("csunhook");
			cdlsym = Invoke<DLSYMFUNC>("dlsym");
			creadHardMemory = Invoke<READHARDMEMORY>("readHardMemory");
			cwriteHardMemory = Invoke<READHARDMEMORY>("writeHardMemory");

			#region 非社区部分内容
			if (COMMERCIAL) {
				cgetStructure = ConvertExtraFunc<GETSTRUCTUREFUNC>("getStructure");
				csetStructure = ConvertExtraFunc<SETSTRUCTUREFUNC>("setStructure");
				cgetPlayerAbilities = ConvertExtraFunc<GETPLAYERABILITIESFUNC>("getPlayerAbilities");
				csetPlayerAbilities = ConvertExtraFunc<RENAMEBYUUIDFUNC>("setPlayerAbilities");
				cgetPlayerAttributes = ConvertExtraFunc<GETPLAYERABILITIESFUNC>("getPlayerAttributes");
				csetPlayerTempAttributes = ConvertExtraFunc<RENAMEBYUUIDFUNC>("setPlayerTempAttributes");
				cgetPlayerMaxAttributes = ConvertExtraFunc<GETPLAYERABILITIESFUNC>("getPlayerMaxAttributes");
				csetPlayerMaxAttributes = ConvertExtraFunc<RENAMEBYUUIDFUNC>("setPlayerMaxAttributes");
				cgetPlayerItems = ConvertExtraFunc<GETPLAYERABILITIESFUNC>("getPlayerItems");
				csetPlayerItems = ConvertExtraFunc<RENAMEBYUUIDFUNC>("setPlayerItems");
				cgetPlayerSelectedItem = ConvertExtraFunc<GETPLAYERABILITIESFUNC>("getPlayerSelectedItem");
				caddPlayerItemEx = ConvertExtraFunc<RENAMEBYUUIDFUNC>("addPlayerItemEx");
				cgetPlayerEffects = ConvertExtraFunc<GETPLAYERABILITIESFUNC>("getPlayerEffects");
				csetPlayerEffects = ConvertExtraFunc<RENAMEBYUUIDFUNC>("setPlayerEffects");
				csetPlayerBossBar = ConvertExtraFunc<SETPLAYERBOSSBARFUNC>("setPlayerBossBar");
				cremovePlayerBossBar = ConvertExtraFunc<RUNCMDFUNC>("removePlayerBossBar");
				ctransferserver = ConvertExtraFunc<TRANSFERSERVERFUNC>("transferserver");
				cteleport = ConvertExtraFunc<TELEPORTFUNC>("teleport");
				csetPlayerSidebar = ConvertExtraFunc<SETPLAYERSIDEBARFUNC>("setPlayerSidebar");
				cremovePlayerSidebar = ConvertExtraFunc<RUNCMDFUNC>("removePlayerSidebar");
				cgetPlayerPermissionAndGametype = ConvertExtraFunc<GETPLAYERABILITIESFUNC>("getPlayerPermissionAndGametype");
				csetPlayerPermissionAndGametype = ConvertExtraFunc<RENAMEBYUUIDFUNC>("setPlayerPermissionAndGametype");
			}
			#endregion
		}

		// 保管一个事件
		private void addcb(string k, EventCab cb)
        {
			ArrayList al;
			if (callbks.TryGetValue(k, out al))
			{
				if (al != null)
					al.Add(cb);
				else
				{
					al = new ArrayList();
					al.Add(cb);
					callbks[k] = al;
				}
			}
			else
			{
				al = new ArrayList();
				al.Add(cb);
				callbks[k] = al;
			}
		}
		// 移除一个事件处理
		private void removecb(string k, EventCab cb)
        {
			ArrayList al;
			if (callbks.TryGetValue(k, out al))
			{
				if (al != null)
					al.Remove(cb);
			}
		}

		/// <summary>
		/// 设置事件发生前监听
		/// </summary>
		/// <param name="key"></param>
		/// <param name="cb"></param>
		/// <returns></returns>
		public bool addBeforeActListener(string key, EventCab cb) {
			bool r = caddBeforeActEvent != null && caddBeforeActEvent(key, cb);
			if (r)
            {
				string k = "Before" + key;
				addcb(k, cb);
			}
			return r;
		}
		
		/// <summary>
		/// 设置事件发生后监听
		/// </summary>
		/// <param name="key"></param>
		/// <param name="cb"></param>
		/// <returns></returns>
		public bool addAfterActListener(string key, EventCab cb) {
			bool r = caddAfterActEvent != null && caddAfterActEvent(key, cb);
			if (r)
            {
				string k = "After" + key;
				addcb(k, cb);
			}
			return r;
		}
		
		/// <summary>
		/// 移除事件发生前监听
		/// </summary>
		/// <param name="key"></param>
		/// <param name="cb"></param>
		/// <returns></returns>
		public bool removeBeforeActListener(string key, EventCab cb) {
			bool r = cremoveBeforeAct != null && cremoveBeforeAct(key, cb);
			if (r)
            {
				string k = "Before" + key;
				removecb(k, cb);
			}
			return r;
		}
		
		/// <summary>
		/// 移除事件发生后监听
		/// </summary>
		/// <param name="key"></param>
		/// <param name="cb"></param>
		/// <returns></returns>
		public bool removeAfterActListener(string key, EventCab cb) {
			bool r = cremoveAfterAct != null && cremoveAfterAct(key, cb);
			if (r)
            {
				string k = "After" + key;
				removecb(k, cb);
			}
			return r;
		}
		
		/// <summary>
		/// 设置共享数据（指针）<br/>
		/// 注：会替换掉旧数据
		/// </summary>
		/// <param name="key">关键字</param>
		/// <param name="data">数据/函数指针</param>
		public void setSharePtr(string key, IntPtr data) {
			if (csetshareptr != null)
				csetshareptr(key, data);
		}
		/// <summary>
		/// 获取共享数据（指针）
		/// </summary>
		/// <param name="key">关键字</param>
		/// <returns></returns>
		public IntPtr getSharePtr(string key) {
			return (cgetSharePtr != null) ? cgetSharePtr(key) :
				IntPtr.Zero;
		}
		/// <summary>
		/// 移除共享数据（指针）
		/// </summary>
		/// <param name="key">关键字</param>
		/// <returns></returns>
		public IntPtr removeSharePtr(string key) {
			return (cremoveSharePtr != null) ? cremoveSharePtr(key) :
				IntPtr.Zero;
		}
		/// <summary>
		/// 设置一个指令说明<br/>
		/// 备注：延期注册的情况，可能不会改变客户端界面
		/// </summary>
		/// <param name="key">命令</param>
		/// <param name="description">描述</param>
		/// <param name="level">执行要求等级</param>
		/// <param name="flag1">命令类型1</param>
		/// <param name="flag2">命令类型2</param>
		public void setCommandDescribeEx(string key, string description, CommandPermissionLevel level, byte flag1, byte flag2) {
			if (csetCommandDescribe != null)
				csetCommandDescribe(key, description, level, flag1, flag2);
		}
		/// <summary>
		/// 设置一个全局指令描述
		/// </summary>
		/// <param name="key">命令</param>
		/// <param name="description">描述</param>
		public void setCommandDescribe(string key, string description) {
			setCommandDescribeEx(key, description, CommandPermissionLevel.Any, (byte)CommandCheatFlag.NotCheat, (byte)CommandVisibilityFlag.Visible);
		}
		
		/// <summary>
		/// 执行后台指令
		/// </summary>
		/// <param name="cmd">语法正确的MC指令</param>
		/// <returns>是否正常执行</returns>
		public bool runcmd(string cmd) {
			return (cruncmd != null) && cruncmd(cmd);
		}
		
		/// <summary>
		/// 发送一条命令输出消息（可被拦截）<br/>
		/// 注：末尾附带换行符
		/// </summary>
		/// <param name="cmdout">待发送的命令输出字符串</param>
		public void logout(string cmdout) {
			if (clogout != null)
				clogout(cmdout);
		}
		
		/// <summary>
		/// 获取在线玩家列表
		/// </summary>
		/// <returns></returns>
		public string getOnLinePlayers() {
			try
            {
				return (cgetOnLinePlayers != null) ? StrTool.c_str(cgetOnLinePlayers()) :
				string.Empty;
			} catch(Exception e) { Console.WriteLine(e.StackTrace); }
			return string.Empty;
		}
		
		/// <summary>
		/// 获取一个结构
		/// </summary>
		/// <param name="did">地图维度</param>
		/// <param name="jsonposa">坐标JSON字符串</param>
		/// <param name="jsonposb">坐标JSON字符串</param>
		/// <param name="exent">是否导出实体</param>
		/// <param name="exblk">是否导出方块</param>
		/// <returns>结构json字符串</returns>
		public string getStructure(int did, string jsonposa, string jsonposb, bool exent, bool exblk) {
			try
			{
				return (cgetStructure != null) ? StrTool.c_str(cgetStructure(did, jsonposa, jsonposb, exent, exblk)) :
				string.Empty;
			}catch (Exception e) { Console.WriteLine(e.StackTrace); }
			return string.Empty;
		}
		/// <summary>
		/// 设置一个结构到指定位置<br/>
		/// 注：旋转类型包含4种有效旋转类型
		/// </summary>
		/// <param name="jdata">结构JSON字符串</param>
		/// <param name="did">地图维度</param>
		/// <param name="jsonposa">起始点坐标JSON字符串</param>
		/// <param name="rot">旋转类型</param>
		/// <param name="exent">是否导入实体</param>
		/// <param name="exblk">是否导入方块</param>
		/// <returns>是否设置成功</returns>
		public bool setStructure(string jdata, int did, string jsonposa, byte rot, bool exent, bool exblk) {
			return (csetStructure != null) && csetStructure(jdata, did, jsonposa, rot, exent, exblk);
		}
		
		/// <summary>
		/// 重命名一个指定的玩家名<br/>
		/// 注：该函数可能不会变更客户端实际显示名
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="newName">新的名称</param>
		/// <returns>是否命名成功</returns>
		public bool reNameByUuid(string uuid, string newName) {
			return (creNameByUuid != null) && creNameByUuid(uuid, newName);
		}
		
		/// <summary>
		/// 获取玩家能力表<br/>
		/// 注：含总计18种能力值
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <returns>能力json字符串</returns>
		public string getPlayerAbilities(string uuid) {
			try
            {
				return (cgetPlayerAbilities != null) ? StrTool.c_str(cgetPlayerAbilities(uuid)) :
				string.Empty;
			}
			catch(Exception e) { Console.WriteLine(e.StackTrace); }
			return string.Empty;
		}
		/// <summary>
		/// 设置玩家能力表<br/>
		/// 注：该函数可能不会变更客户端实际显示能力
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="abdata">新能力json数据字符串</param>
		/// <returns>是否设置成功</returns>
		public bool setPlayerAbilities(string uuid, string abdata) {
			return (csetPlayerAbilities != null) && csetPlayerAbilities(uuid, abdata);
		}
		
		/// <summary>
		/// 获取玩家属性表<br/>
		/// 注：总计14种生物属性，含部分有效玩家属性
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <returns>属性json字符串</returns>
		public string getPlayerAttributes(string uuid) {
			try
            {
				return (cgetPlayerAttributes != null) ? StrTool.c_str(cgetPlayerAttributes(uuid)) :
				string.Empty;
			} catch (Exception e) { Console.WriteLine(e.StackTrace); }
			return string.Empty;
			
		}
		/// <summary>
		/// 设置玩家属性临时值表<br/>
		/// 注：该函数可能不会变更客户端实际显示值
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="newTempAttributes">新属性临时值json数据字符串</param>
		/// <returns>是否设置成功</returns>
		public bool setPlayerTempAttributes(string uuid, string newTempAttributes) {
			return (csetPlayerTempAttributes != null) && csetPlayerTempAttributes(uuid, newTempAttributes);
		}
		/// <summary>
		/// 获取玩家属性上限值表
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <returns>属性上限值json字符串</returns>
		public string getPlayerMaxAttributes(string uuid) {
			try
            {
				return (cgetPlayerMaxAttributes != null) ? StrTool.c_str(cgetPlayerMaxAttributes(uuid)) :
				string.Empty;
			} catch (Exception e) { Console.WriteLine(e.StackTrace); }
			return string.Empty;
		}
		/// <summary>
		/// 设置玩家属性上限值表<br/>
		/// 注：该函数可能不会变更客户端实际显示值
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="newMaxAttributes">新属性上限值json数据字符串</param>
		/// <returns>是否设置成功</returns>
		public bool setPlayerMaxAttributes(string uuid, string newMaxAttributes) {
			return (csetPlayerMaxAttributes != null) && csetPlayerMaxAttributes(uuid, newMaxAttributes);
		}
		
		/// <summary>
		/// 获取玩家所有物品列表<br/>
		/// 注：玩家物品包括末影箱、装备、副手和背包四项物品的nbt描述型数据列表。nbt被序列化数据类型的tag所描述，总计12种有效tag，所对应值可序列化为json数据，亦可反序列化为nbt
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <returns>物品列表json字符串</returns>
		public string getPlayerItems(string uuid) {
			try
            {
				return (cgetPlayerItems != null) ? StrTool.c_str(cgetPlayerItems(uuid)) :
				string.Empty;
			} catch(Exception e) { Console.WriteLine(e.StackTrace); }
			return string.Empty;
		}
		/// <summary>
		/// 设置玩家所有物品列表<br/>
		/// 注：特定条件下可能不会变更游戏内实际物品
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="newItems">新物品列表json数据字符串</param>
		/// <returns>是否设置成功</returns>
		public bool setPlayerItems(string uuid, string newItems) {
			return (csetPlayerItems != null) && csetPlayerItems(uuid, newItems);
		}
		/// <summary>
		/// 获取玩家当前选中项信息<br/>
		/// 注：选中项包含选中框所处位置，以及选中物品的nbt描述型数据
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <returns>当前选中项信息json字符串</returns>
		public string getPlayerSelectedItem(string uuid) {
			try
            {
				return (cgetPlayerSelectedItem != null) ? StrTool.c_str(cgetPlayerSelectedItem(uuid)) :
				string.Empty;
			} catch (Exception e) { Console.WriteLine(e.StackTrace); }
			return string.Empty;
		}
		/// <summary>
		/// 增加玩家一个物品<br/>
		/// 注：特定条件下可能不会变更游戏内实际物品
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="item">物品json数据字符串</param>
		/// <returns>是否添加成功</returns>
		public bool addPlayerItemEx(string uuid, string item) {
			return (caddPlayerItemEx != null) && caddPlayerItemEx(uuid, item);
		}
		/// <summary>
		/// 增加玩家一个物品<br/>
		/// 注：特定条件下可能不会变更游戏内实际物品
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="id">物品id值</param>
		/// <param name="aux">物品特殊值</param>
		/// <param name="count">数量</param>
		/// <returns>是否增加成功</returns>
		public bool addPlayerItem(string uuid, int id, short aux, byte count) {
			return (caddPlayerItem != null) && caddPlayerItem(uuid, id, aux, count);
		}
		/// <summary>
		/// 获取玩家所有效果列表
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <returns>效果列表json字符串</returns>
		public string getPlayerEffects(string uuid) {
			try
            {
				return (cgetPlayerEffects != null) ? StrTool.c_str(cgetPlayerEffects(uuid)) :
				string.Empty;
			} catch (Exception e) { Console.WriteLine(e.StackTrace); }
			return string.Empty;
		}
		/// <summary>
		/// 设置玩家所有效果列表<br/>
		/// 注：特定条件下可能不会变更游戏内实际界面
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="newEffects">新效果列表json数据字符串</param>
		/// <returns>是否设置成功</returns>
		public bool setPlayerEffects(string uuid, string newEffects) {
			return (csetPlayerEffects != null) && csetPlayerEffects(uuid, newEffects);
		}
		
		/// <summary>
		/// 设置玩家自定义血条
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="title">血条标题</param>
		/// <param name="percent">血条百分比</param>
		/// <returns></returns>
		public bool setPlayerBossBar(string uuid, string title, float percent) {
			return (csetPlayerBossBar != null) && csetPlayerBossBar(uuid, title, percent);
		}
		/// <summary>
		/// 清除玩家自定义血条
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <returns>是否清除成功</returns>
		public bool removePlayerBossBar(string uuid) {
			return (cremovePlayerBossBar != null) && cremovePlayerBossBar(uuid);
		}
		
		/// <summary>
		/// 查询在线玩家基本信息
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <returns>玩家基本信息json字符串</returns>
		public string selectPlayer(string uuid) {
			try
            {
				return (cselectPlayer != null) ? StrTool.c_str(cselectPlayer(uuid)) :
				string.Empty;
			} catch (Exception e) { Console.WriteLine(e); }
			return string.Empty;
		}
		
		/// <summary>
		/// 传送玩家至指定服务器
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="addr">待传服务器</param>
		/// <param name="port">端口</param>
		/// <returns>是否传送成功</returns>
		public bool transferserver(string uuid, string addr, int port) {
			return (ctransferserver != null) && ctransferserver(uuid, addr, port);
		}
		/// <summary>
		/// 传送玩家至指定坐标和维度
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <param name="did">维度ID</param>
		/// <returns>是否传送成功</returns>
		public bool teleport(string uuid, float x, float y, float z, int did) {
			return (cteleport != null) && cteleport(uuid, x, y, z, did);
		}
		/// <summary>
		/// 模拟玩家发送一个文本
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="msg">待模拟发送的文本</param>
		/// <returns>是否发送成功</returns>
		public bool talkAs(string uuid, string msg) {
			return (ctalkAs != null) && ctalkAs(uuid, msg);
		}
		/// <summary>
		/// 模拟玩家执行一个指令
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="cmd">待模拟执行的指令</param>
		/// <returns>是否发送成功</returns>
		public bool runcmdAs(string uuid, string cmd) {
			return (cruncmdAs != null) && cruncmdAs(uuid, cmd);
		}

		/// <summary>
		/// 断开一个玩家的连接
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="tips">断开提示（设空值则为默认值）</param>
		/// <returns></returns>
		public bool disconnectClient(string uuid, string tips) {
			return (cdisconnectClient != null) && cdisconnectClient(uuid, tips);
		}

		/// <summary>
		/// 发送一个原始显示文本给玩家
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="msg">文本内容，空白内容则不予发送</param>
		/// <returns>是否发送成功</returns>
		public bool sendText(string uuid, string msg)
        {
			return (csendText != null) && csendText(uuid, msg);
		}

		/// <summary>
		/// 向指定的玩家发送一个简单表单
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="title">表单标题</param>
		/// <param name="content">内容</param>
		/// <param name="buttons">按钮文本数组字符串</param>
		/// <returns>创建的表单id，为 0 表示发送失败</returns>
		public uint sendSimpleForm(string uuid, string title, string content, string buttons) {
			return (csendSimpleForm != null) ? csendSimpleForm(uuid, title, content, buttons) :
				0;
		}
		/// <summary>
		/// 向指定的玩家发送一个模式对话框
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="title">表单标题</param>
		/// <param name="content">内容</param>
		/// <param name="button1">按钮1标题（点击该按钮selected为true）</param>
		/// <param name="button2">按钮2标题（点击该按钮selected为false）</param>
		/// <returns>创建的表单id，为 0 表示发送失败</returns>
		public uint sendModalForm(string uuid, string title, string content, string button1, string button2) {
			return (csendModalForm != null) ? csendModalForm(uuid, title, content, button1, button2) :
				0;
		}
		/// <summary>
		/// 向指定的玩家发送一个自定义表单
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="json">自定义表单的json字符串<br/>
		/// （要使用自定义表单类型，参考nk、pm格式或minebbs专栏）</param>
		/// <returns>创建的表单id，为 0 表示发送失败</returns>
		public uint sendCustomForm(string uuid, string json) {
			return (csendCustomForm != null) ? csendCustomForm(uuid, json) :
				0;
		}
		/// <summary>
		/// 放弃一个表单<br/>
		/// 注：已被接收到的表单会被自动释放
		/// </summary>
		/// <param name="formid">表单id</param>
		/// <returns>是否释放成功</returns>
		public bool releaseForm(uint formid) {
			return (creleaseForm != null) && creleaseForm(formid);
		}
		
		/// <summary>
		/// 设置玩家自定义侧边栏临时计分板<br/>
		/// 注：列表总是从第1行开始，总计不超过15行
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="title">侧边栏标题</param>
		/// <param name="list">列表字符串数组</param>
		/// <returns>是否设置成功</returns>
		public bool setPlayerSidebar(string uuid, string title, string list) {
			return (csetPlayerSidebar != null) && csetPlayerSidebar(uuid, title, list);
		}
		/// <summary>
		/// 清除玩家自定义侧边栏
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <returns>是否清除成功</returns>
		public bool removePlayerSidebar(string uuid) {
			return (cremovePlayerSidebar != null) && cremovePlayerSidebar(uuid);
		}
		
		/// <summary>
		/// 获取玩家权限与游戏模式<br/>
		/// 注：OP命令等级包含6个有效等级[op-permission-level]，权限包含3种有效权限[permissions.json]，游戏模式包含5种有效模式[gamemode]
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <returns>权限与模式的json字符串</returns>
		public string getPlayerPermissionAndGametype(string uuid) {
			try
            {
				return (cgetPlayerPermissionAndGametype != null) ? StrTool.c_str(cgetPlayerPermissionAndGametype(uuid)) :
				string.Empty;
			} catch (Exception e) { Console.WriteLine(e.StackTrace); }
			return string.Empty;
		}
		/// <summary>
		/// 设置玩家权限与游戏模式<br/>
		/// 注：特定条件下可能不会变更游戏内实际能力
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="newModes">新权限或模式json数据字符串</param>
		/// <returns>是否设置成功</returns>
		public bool setPlayerPermissionAndGametype(string uuid, string newModes) {
			return (csetPlayerPermissionAndGametype != null) && csetPlayerPermissionAndGametype(uuid, newModes);
		}

		// 社区贡献

		/// <summary>
		/// 获取指定玩家指定计分板上的数值<br/>
		/// 注：特定情况下会自动创建计分板
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="objname">计分板登记的名称</param>
		/// <returns>获取的目标值，若目标不存在则返回0</returns>
		public int getscoreboard(string uuid, string objname)
        {
			return (cgetscoreboardValue != null) ? cgetscoreboardValue(uuid, objname) :
				0;
        }
		/// <summary>
		/// 设置指定玩家指定计分板上的数值
		/// </summary>
		/// <param name="uuid">在线玩家的uuid字符串</param>
		/// <param name="objname">计分板登记的名称，若不存在则自动添加</param>
		/// <param name="count">待设定的目标值</param>
		/// <returns>是否设置成功</returns>
		public bool setscoreboard(string uuid, string objname, int count)
        {
			return csetscoreboardValue != null && csetscoreboardValue(uuid, objname, count);
		}
		/// <summary>
		/// 设置服务器的显示名信息<br/>
		/// （注：服务器名称加载时机在地图完成载入之后）
		/// </summary>
		/// <param name="motd">新服务器显示名信息</param>
		/// <param name="isShow">是否公开显示</param>
		/// <returns>是否设置成功</returns>
		public bool setServerMotd(string motd, bool isShow)
        {
			return csetServerMotd != null && csetServerMotd(motd, isShow);
        }

		// 底层相关

		/// <summary>
		/// 设置一个钩子
		/// </summary>
		/// <param name="rva">原型函数相对地址</param>
		/// <param name="hook">新函数</param>
		/// <param name="org">待保存原型函数的指针</param>
		/// <returns></returns>
		public bool cshook(int rva, IntPtr hook, out IntPtr org)
        {
			IntPtr sorg = IntPtr.Zero;
            var ret = ccshook != null && ccshook(rva, hook, out sorg);
            org = sorg;
            return ret;
        }
		/// <summary>
		/// 卸载一个钩子
		/// </summary>
		/// <param name="hook">待卸载的函数</param>
		/// <param name="org">已保存了原型函数的指针</param>
		/// <returns></returns>
		public bool csunhook(IntPtr hook, ref IntPtr org) {
			IntPtr sorg = org;
			var ret = ccsunhook != null && ccsunhook(hook, out sorg);
			org = sorg;
			return ret;
		}
		/// <summary>
		/// 取相对地址对应的实际指针
		/// </summary>
		/// <param name="rva"></param>
		/// <returns></returns>
		public IntPtr dlsym(int rva) {
			return cdlsym != null ? cdlsym(rva) :
				IntPtr.Zero;
		}

		/// <summary>
		/// 读特定段内存硬编码
		/// </summary>
		/// <param name="rva">函数片段起始位置相对地址</param>
		/// <param name="size">内存长度</param>
		/// <returns></returns>
		public byte[] readHardMemory(int rva, int size) {
			byte[] x = new byte[size];
			if (creadHardMemory != null)
				if (creadHardMemory(rva, x, size))
					return x;
			return null;
        }

		/// <summary>
		/// 写特定段内存硬编码
		/// </summary>
		/// <param name="rva">函数片段起始位置相对地址</param>
		/// <param name="data">新数据内容</param>
		/// <param name="size">内存长度</param>
		/// <returns></returns>
		public bool writeHardMemory(int rva, byte[] data, int size)
		{
			return (cwriteHardMemory != null) && cwriteHardMemory(rva, data, size);
		}
	}
}
