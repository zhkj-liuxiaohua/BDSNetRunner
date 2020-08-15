/*
 * 由SharpDevelop创建。
 * 用户： BDSNetRunner
 * 日期: 2020/7/18
 * 时间: 12:32
 * 
 * 要改变这种模板请点击 工具|选项|代码编写|编辑标准头文件
 */
using System;

namespace CSR
{
	partial class Plugin
	{
		private static MCCSAPI mapi = null;
		/// <summary>
		/// 静态api对象
		/// </summary>
		public static MCCSAPI api { get { return mapi; } }
		#region 插件统一调用接口，请勿随意更改
		public static int onServerStart(string pathandversion) {
			string path = null, version = null;
			bool commercial = false;
			string [] pav = pathandversion.Split(',');
			if (pav.Length > 1) {
				path = pav[0];
				version = pav[1];
				commercial = (pav[pav.Length - 1] == "1");
				mapi = new MCCSAPI(path, version, commercial);
				if (mapi != null) {
					onStart(mapi);
					GC.KeepAlive(mapi);
					return 0;
				}
			}
			Console.WriteLine("Load failed.");
			return -1;
		}
		#endregion

		~Plugin()
        {
			//Console.WriteLine("[CSR Plugin] Ref released.");
        }

		#region 必要接口 onStart ，由用户实现
		// public static void onStart(MCCSAPI api)
		#endregion
	}
}