#pragma once

#include <Windows.h>
#include <iostream>
#include "./Detours/include/detours.h"
#include <unordered_map>

using VA = unsigned __int64;
using RVA = unsigned int;

template<typename Type>
using Ptr = Type*;

//----------------------------------
// Hook 机制
//----------------------------------
enum class HookErrorCode {
	ERR_SUCCESS,
	ERR_TRANSACTION_BEGIN,
	ERR_UPDATE_THREAD,
	ERR_ATTACH,
	ERR_DETACH,
	ERR_TRANSACTION_COMMIT
};
template<typename T = Ptr<void>>
auto Hook(Ptr<T> p, T f) {
	int error = DetourTransactionBegin();
	if (error != NO_ERROR) {
		return HookErrorCode::ERR_TRANSACTION_BEGIN;
	}
	error = DetourUpdateThread(GetCurrentThread());
	if (error != NO_ERROR) {
		return HookErrorCode::ERR_UPDATE_THREAD;
	}
	error = DetourAttach(
		reinterpret_cast<Ptr<PVOID>>(p),
		reinterpret_cast<PVOID>(f)
	);
	if (error != NO_ERROR) {
		return HookErrorCode::ERR_ATTACH;
	}
	error = DetourTransactionCommit();
	if (error != NO_ERROR) {
		return HookErrorCode::ERR_TRANSACTION_COMMIT;
	}
	return HookErrorCode::ERR_SUCCESS;
}
template<typename T = Ptr<void>>
auto UnHook(Ptr<T> p, T f) {
	int error = DetourTransactionBegin();
	if (error != NO_ERROR) {
		return HookErrorCode::ERR_TRANSACTION_BEGIN;
	}
	error = DetourUpdateThread(GetCurrentThread());
	if (error != NO_ERROR) {
		return HookErrorCode::ERR_UPDATE_THREAD;
	}
	error = DetourDetach(
		reinterpret_cast<Ptr<PVOID>>(p),
		reinterpret_cast<PVOID>(f)
	);
	if (error != NO_ERROR) {
		return HookErrorCode::ERR_DETACH;
	}
	error = DetourTransactionCommit();
	if (error != NO_ERROR) {
		return HookErrorCode::ERR_TRANSACTION_COMMIT;
	}
	return HookErrorCode::ERR_SUCCESS;
}


//----------------------------------
// THook 模板
//----------------------------------
template<typename T_VA, typename T_RVA>
struct RegisterStaticHook {
	RegisterStaticHook(T_RVA sym, void* hook, void** org) {
		auto base = reinterpret_cast<T_VA>(GetModuleHandle(NULL));
		*org = reinterpret_cast<void*>(base + sym);
		auto ret = Hook<void*>(org, hook);
		if (ret != HookErrorCode::ERR_SUCCESS) {
			std::cout << "[Error] ";
			switch (ret) {
			case HookErrorCode::ERR_TRANSACTION_BEGIN:
				std::cout << "DetourTransactionBegin";
				break;
			case HookErrorCode::ERR_UPDATE_THREAD:
				std::cout << "DetourUpdateThread";
				break;
			case HookErrorCode::ERR_ATTACH:
				std::cout << "DetourAttach";
				break;
			case HookErrorCode::ERR_DETACH:
				std::cout << "DetourDetach";
				break;
			case HookErrorCode::ERR_TRANSACTION_COMMIT:
				std::cout << "DetourTransactionCommit";
				break;
			}
			std::cout << "failed!" << std::endl;
		}
	}
	// workaround for a warning
	template <typename T>
	RegisterStaticHook(T_RVA sym, T hook, void** org) {
		union {
			T a;
			void* b;
		} hookUnion;
		hookUnion.a = hook;
		RegisterStaticHook(sym, hookUnion.b, org);
	}
};

//----------------------------------
// THook 普通函数、静态类方法 系列
//----------------------------------
#define _TStaticHook(pclass, iname, sym, ret, ...)\
struct _TStaticHook_##iname pclass {\
	static ret (*_original)(__VA_ARGS__);\
	template <typename... Params> static ret original(Params &&... params) { return (*_original)(std::forward<Params>(params)...); }\
	static ret _hook(__VA_ARGS__);\
};\
static RegisterStaticHook<VA, RVA> _TRStaticHook_##iname(sym, &_TStaticHook_##iname::_hook, (void **)&_TStaticHook_##iname::_original);\
ret (*_TStaticHook_##iname::_original)(__VA_ARGS__);\
ret _TStaticHook_##iname::_hook(__VA_ARGS__)

#define _TStaticDefHook(iname, sym, ret, type, ...)		_TStaticHook( : public type, iname, sym, ret, ##__VA_ARGS__)
#define TStaticHook2(iname, ret, sym, type, ...)		_TStaticDefHook(iname, sym, ret, type, ##__VA_ARGS__)
#define TStaticHook(ret, sym, type, ...)				TStaticHook2(sym, ret, sym, type, ##__VA_ARGS__)

#define _TStaticNoDefHook(iname, sym, ret, ...)			_TStaticHook(, iname, sym, ret, ##__VA_ARGS__)
#define THook2(iname, ret, sym, ...)					_TStaticNoDefHook(iname, sym, ret, ##__VA_ARGS__)
#define THook(ret, sym, ...)							THook2(sym, ret, sym, ##__VA_ARGS__)

//----------------------------------
// THook 非静态类方法 系列
//----------------------------------
#define _TInstanceHook(class_inh, pclass, iname, sym, ret, ...)\
struct _TInstanceHook_##iname class_inh {\
	static ret (_TInstanceHook_##iname::*_original)(__VA_ARGS__);\
	template <typename... Params> static ret original(pclass *_this, Params &&... params) {\
		return (((_TInstanceHook_##iname *)_this)->*_original)(std::forward<Params>(params)...);\
	}\
	ret _hook(__VA_ARGS__);\
};\
static RegisterStaticHook<VA, RVA> _TRInstanceHook_##iname(sym, &_TInstanceHook_##iname::_hook, (void **)&_TInstanceHook_##iname::_original);\
ret (_TInstanceHook_##iname::*_TInstanceHook_##iname::_original)(__VA_ARGS__);\
ret _TInstanceHook_##iname::_hook(__VA_ARGS__)

#define _TInstanceDefHook(iname, sym, ret, type, ...)	_TInstanceHook( : public type, type, iname, sym, ret, ##__VA_ARGS__)
#define TInstanceHook2(iname, ret, sym, type, ...)		_TInstanceDefHook(iname, sym, ret, type, ##__VA_ARGS__)
#define TInstanceHook(ret, sym, type, ...)				TInstanceHook2(sym, ret, sym, type, ##__VA_ARGS__)

#define _TInstanceNoDefHook(iname, sym, ret, ...)		_TInstanceHook(, void, iname, sym, ret, ##__VA_ARGS__)
#define TClasslessInstanceHook2(iname, ret, sym, ...)	_TInstanceNoDefHook(iname, sym, ret, ##__VA_ARGS__)
#define TClasslessInstanceHook(ret, sym, ...)			TClasslessInstanceHook2(sym, ret, sym, ##__VA_ARGS__)

//----------------------------------
// 其他
//----------------------------------
#define REF(PTR)										(&PTR)
#define DEREF(PTR)										(*PTR)
#define OBJECT(TYPE, VALUE)								reinterpret_cast<TYPE>(VALUE)

#define POINTER(PTR_TYPE, PTR)							OBJECT(PTR_TYPE, PTR)
#define POINTER_ADD_OFFSET(TYPE, PTR, OFFSET)			POINTER(TYPE, POINTER(VA, PTR)+OFFSET)
#define CLASS_OBJECT(TYPE, THISPTR, OFFSET)				DEREF(POINTER_ADD_OFFSET(Ptr<TYPE>, THISPTR, OFFSET))
#define CLASS_VTABLE_OBJECT(TYPE, THISPTR, OFFSET)		DEREF(POINTER(Ptr<TYPE>, DEREF(POINTER(Ptr<VA>, THISPTR))+OFFSET))

template<typename T_RET, typename... Args>
T_RET Symcall(RVA sym_rva, Args... args) {
	using FnType = T_RET(*)(Args...);
	FnType p = POINTER_ADD_OFFSET(FnType, GetModuleHandle(NULL), sym_rva);
	return p(args...);
}
#define SYMCALL(RETURN_TYPE, SYM_RVA, ...)				(Symcall<RETURN_TYPE>(SYM_RVA, ##__VA_ARGS__))
#define SYM_POINT(TYPE, SYM_RVA)						POINTER_ADD_OFFSET(Ptr<TYPE>, GetModuleHandle(NULL), SYM_RVA)
#define SYM_OBJECT(TYPE, SYM_RVA)						DEREF(POINTER_ADD_OFFSET(Ptr<TYPE>, GetModuleHandle(NULL), SYM_RVA))
