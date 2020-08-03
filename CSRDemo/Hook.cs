using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Threading;
using System.Collections.Generic;
using CSR;


namespace CSRDemo
{
	/// <summary>
	/// 此类为hook调用示范类，步骤如下：<br/>
	/// 1. 定义目标函数原型的托管；<br/>
	/// 2. 创建函数原型存储指针；<br/>
	/// 3. 构造新方法，用以替代目标函数；<br/>
	/// 4. 调用hook，将位于指定RVA位置的目标函数替换为新方法调用。<br/>
	/// 其它方法可仿照此类进行构造，不再赘述。
	/// </summary>
	public static class THook {
		public static Dictionary<string, ArrayList> RVAs = new Dictionary<string, ArrayList>();
		
		private static MCCSAPI mcapi = null;

		delegate ulong CMD_REG_Func(ulong a1, ulong a2, ulong a3, byte level, byte f1, byte f2);
		private static IntPtr _CS_REGISTERCOMMAND_org = IntPtr.Zero;
		/// <summary>
		/// 新方法，修改注册的命令标识，所有指令全部增加一个无需作弊可用的flag
		/// </summary>
		/// <param name="a1">CommandRegistry句柄</param>
		/// <param name="a2">指令原型</param>
		/// <param name="a3">指令描述</param>
		/// <param name="level">需求op等级</param>
		/// <param name="f1">指令属性flag1</param>
		/// <param name="f2">指令属性flag2</param>
		/// <returns></returns>
		private static readonly CMD_REG_Func cs_reghookptr = (ulong a1, ulong a2, ulong a3, byte level, byte f1, byte f2) =>{
			f1 |= (byte) MCCSAPI.CommandCheatFlag.NotCheat;
			var org = Marshal.GetDelegateForFunctionPointer(_CS_REGISTERCOMMAND_org, typeof(CMD_REG_Func)) as CMD_REG_Func;
			return org(a1, a2, a3, level, f1, f2);
		};

		delegate ulong ONCREATEPLAYER_Func(ulong a1, IntPtr a2);
		private static IntPtr _CS_ONCREATEPLAYER_org = IntPtr.Zero;
		/// <summary>
		/// 新方法，监听第一个玩家进入游戏，并设置玩家的OP等级
		/// </summary>
		/// <param name="a1">ServerScoreboard句柄</param>
		/// <param name="player">player指针</param>
		/// <returns></returns>
		private static readonly ONCREATEPLAYER_Func cs_crthookptr = (ulong a1, IntPtr player) =>{
			Console.WriteLine("[c# hook] A player is join.");
			Symcall.setPermission(mcapi, player, 3);	// 若参数为3，则op模式可使用kick
			Thread t = new Thread(releasehook);         // 延时卸载钩子，也可于当前线程末尾时机进行卸载，也可不卸载
			t.Start();
			ONCREATEPLAYER_Func org = Marshal.GetDelegateForFunctionPointer(_CS_ONCREATEPLAYER_org, typeof(ONCREATEPLAYER_Func)) as ONCREATEPLAYER_Func;
			return org(a1, player);
		};

		// 初始化hook
		public static void init(MCCSAPI api) {
			mcapi = api;
			// 初始化RVA，或可远程获取
			ArrayList al = new ArrayList(new int[]{0x00B9D4C0, 0x00429850, 0x004ECFD0});
			RVAs["1.16.1.2"] = al;
			ArrayList a2 = new ArrayList(new int[] {0x00B9D100, 0x00429820, 0x004ECFA0});
			RVAs["1.16.10.2"] = a2;
			try {
				ArrayList rval = null;
				if (RVAs.TryGetValue(api.VERSION, out rval)) {
					if (rval != null && rval.Count > 0) {
						IntPtr tmpCrtorg = IntPtr.Zero;
						api.cshook((int)rval[0],	// IDA ServerScoreboard::onPlayerJoined
							Marshal.GetFunctionPointerForDelegate(cs_crthookptr), out tmpCrtorg);
						_CS_ONCREATEPLAYER_org = tmpCrtorg;
								
						tmpCrtorg = IntPtr.Zero;
						api.cshook((int)rval[1],	// IDA CommandRegistry::registerCommand
							Marshal.GetFunctionPointerForDelegate(cs_reghookptr), out tmpCrtorg);
						_CS_REGISTERCOMMAND_org = tmpCrtorg;
					}
				}
			} catch (Exception e) {
				Console.WriteLine(e.StackTrace);
			}
		}
		
		// 休眠1s后卸载钩子
		private static void releasehook() {
			Thread.Sleep(1000);
			if (mcapi != null) {
				bool unhooked = mcapi.csunhook(Marshal.GetFunctionPointerForDelegate(cs_crthookptr),
					ref _CS_ONCREATEPLAYER_org);
				if (unhooked) {
					Console.WriteLine("[C# unhook] release hook join listen.");
				}
			}
		}
	}
	
	/// <summary>
	/// 此类为symcall示范类，步骤如下：<br/>
	/// 1. 创建函数原型；<br/>
	/// 2. 获取目标RVA位置处的函数指针；<br/>
	/// 3. 函数调用
	/// </summary>
	public static class Symcall {

		delegate void SETPERMISSION_FUNC(IntPtr p, byte per);
		/// <summary>
		/// SYMCALL方式设置玩家OP等级
		/// </summary>
		/// <param name="api"></param>
		/// <param name="player">ServerPlayer指针</param>
		/// <param name="per">等级</param>
		public static void setPermission(MCCSAPI api, IntPtr player, byte per) {
			Console.WriteLine("[CS] setPlayer OP level to {0}", per);
			IntPtr org = api.dlsym((int)THook.RVAs[api.VERSION][2]);	// IDA ServerPlayer::setPermissions
			SETPERMISSION_FUNC func = (SETPERMISSION_FUNC)Marshal.GetDelegateForFunctionPointer(org, typeof(SETPERMISSION_FUNC));
			func(player, per);
		}
	}
}
