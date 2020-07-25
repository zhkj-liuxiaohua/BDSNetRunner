// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#pragma comment(lib, "./Detours/lib.X64/detours.lib")
#pragma comment(lib, "./json/lib/jsoncpp.lib")
#pragma comment ( lib, "ws2_32.lib")
#pragma comment ( lib, "wldap32.lib")
#pragma comment ( lib, "Crypt32.lib")
#pragma comment ( lib, "Normaliz.lib")

#include "mod.h"



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        init();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        exit();
        break;
    }
    return TRUE;
}

