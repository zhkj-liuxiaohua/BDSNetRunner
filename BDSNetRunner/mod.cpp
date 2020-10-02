#include "mod.h"
#include "THook.h"
#include <stdio.h>
#include <iostream>
#include "BDS.hpp"
#include "Component.h"
#include <thread>
#include <map>
#include <fstream>
#include <mscoree.h>
#include <metahost.h>
#include "json/json.h"
#include "tick/tick.h"
#include "GUI/SimpleForm.h"
#include <mutex>
#include "commands/commands.h"
#pragma comment(lib, "mscoree.lib")

// 当前插件平台版本号
static const wchar_t* VERSION = L"1.16.40.2";
static const wchar_t* ISFORCOMMERCIAL = L"1";

static bool netregok = false;

static void releaseNetFramework();
void** getOriginalData(void*);
HookErrorCode mTHook2(RVA, void*);

static std::mutex mleftlock;

// 调试信息
template<typename T>
static void PR(T arg) {
#ifndef RELEASED
	std::cout << arg << std::endl;
#endif // !RELEASED
}

// 转换Json对象为字符串
static std::string toJsonString(Json::Value v) {
	Json::StreamWriterBuilder w;
	std::ostringstream os;
	std::unique_ptr<Json::StreamWriter> jsonWriter(w.newStreamWriter());
	jsonWriter->write(v, &os);
	return std::string(os.str());
}

// 转换字符串为Json对象
static Json::Value toJson(std::string s) {
	Json::Value jv;
	Json::CharReaderBuilder r;
	JSONCPP_STRING errs;
	std::unique_ptr<Json::CharReader> const jsonReader(r.newCharReader());
	bool res = jsonReader->parse(s.c_str(), s.c_str() + s.length(), &jv, &errs);
	if (!res || !errs.empty()) {
		PR(u8"JSON转换失败。。" + errs);
	}
	return jv;
}

// UTF-8 转 GBK
static std::string UTF8ToGBK(const char* strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[(size_t)len + 1];
	memset(wszGBK, 0, (size_t)len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[(size_t)len + 1];
	memset(szGBK, 0, (size_t)len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

// GBK 转 UTF-8
static std::string GBKToUTF8(const char* strGBK)
{
	std::string strOutUTF8 = "";
	WCHAR* str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char* str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}

// 自动复制字符
static void autoByteCpy(char** d, const char* s) {
	if (d) {
		VA l = strlen(s);
		if (*d) {
			delete (*d);
		}
		*d = new char[l + 1]{ 0 };
		strcpy_s(*d, l + 1, s);
	}
}

static VA p_spscqueue = 0;

static VA p_level = 0;

static VA p_ServerNetworkHandle = 0;

static HMODULE GetSelfModuleHandle()
{
	MEMORY_BASIC_INFORMATION mbi;
	return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0) ? (HMODULE)mbi.AllocationBase : NULL);
}
// 获取自身DLL路径
static std::wstring GetDllPathandVersion() {
	std::ifstream file;
	wchar_t curDir[256]{ 0 };
	GetModuleFileName(GetSelfModuleHandle(), curDir, 256);
	std::wstring dllandVer = std::wstring(curDir);
	dllandVer = dllandVer + std::wstring(L",") + std::wstring(VERSION);
	if (netregok)
		dllandVer = dllandVer + std::wstring(L",") + std::wstring(ISFORCOMMERCIAL);
	return dllandVer;
}

std::unordered_map<std::string, std::vector<void*>*> beforecallbacks, aftercallbacks;

// 执行回调
static bool runCscode(std::string key, ActMode mode, Events& eventData) {
	auto& funcs = (mode == ActMode::BEFORE) ? beforecallbacks :
		aftercallbacks;
	auto dv = funcs[key];
	bool ret = true;
	if (dv) {
		if (dv->size() > 0) {
			for (auto& func : *dv) {
				try {
					ret = ret && ((bool(*)(Events))func)(eventData);
				}
				catch (...) { PR("[CSR] Event callback exception."); }
			}
		}
	}
	return ret;
}

static ACTEVENT ActEvent;

static std::unordered_map<std::string, VA*> sListens;		// 监听列表
static std::unordered_map<std::string, bool> isListened;	// 是否已监听

static bool addListener(std::string key, ActMode mode, bool(*func)(Events)) {
	auto& funcs = (mode == ActMode::BEFORE) ? beforecallbacks :
		aftercallbacks;
	if (key != "" && func != NULL) {
		if (!isListened[key]) {			// 动态挂载hook监听
			VA* syms = sListens[key];
			int hret = 0;
			if (syms) {
				for (VA i = 0, len = syms[0]; i < len; ++i) {								// 首数为sym长度
					hret += (int)mTHook2((RVA)syms[(i * 2) + 1], (void*)syms[(i * 2) + 2]);	// 第一数为sym，第二数为func
				}
			}
			if (hret) {
				PR("[CSR] Some hooks wrong at event setting.");
			}
			isListened[key] = true;		// 只挂载一次
		}
		auto dv = funcs[key];
		if (dv == NULL) {
			dv = new std::vector<void*>();
			funcs[key] = dv;
		}
		if (std::find(dv->begin(), dv->end(), func) == dv->end()) {
			// 未找到已有函数，加入回调
			dv->push_back(func);
			return true;
		}
	}
	return false;
}

static void remove_v2(std::vector<void*>* v, void* val) {
	v->erase(std::remove(v->begin(), v->end(), val), v->end());
}

static bool removeListener(std::string key, ActMode mode, bool(*func)(Events)) {
	auto& funcs = (mode == ActMode::BEFORE) ? beforecallbacks :
		aftercallbacks;
	if (key != "" && func != NULL) {
		auto dv = funcs[key];
		if (dv != NULL) {
			bool exi = std::find(dv->begin(), dv->end(), func) != dv->end();
			if (exi)
				remove_v2(dv, func);
			return exi;
		}
	}
	return false;
}

bool addBeforeActListener(const char* key, bool(*func)(Events))
{
	return addListener(key, ActMode::BEFORE, func);
}

bool addAfterActListener(const char* key, bool(*func)(Events))
{
	return addListener(key, ActMode::AFTER, func);
}

bool removeBeforeActListener(const char* key, bool(*func)(Events)) {
	return removeListener(key, ActMode::BEFORE, func);
}

bool removeAfterActListener(const char* key, bool(*func)(Events)) {
	return removeListener(key, ActMode::AFTER, func);
}

static std::unordered_map<std::string, void*> shareData;

void setSharePtr(const char* key, void* func)
{
	shareData[key] = func;
}

void* getSharePtr(const char* key) {
	return shareData[key];
}

void* removeSharePtr(const char* key) {
	std::string k = std::string(key);
	void* x = shareData[k];
	shareData.erase(k);
	return x;
}



// 维度ID转换为中文字符
static std::string toDimenStr(int dimensionId) {
	switch (dimensionId) {
	case 0:return u8"主世界";
	case 1:return u8"地狱";
	case 2:return u8"末地";
	default:
		break;
	}
	return u8"未知维度";
}


static VA regHandle = 0;

static struct CmdDescriptionFlags {
	std::string description;
	char level;
	char flag1;
	char flag2;
};

static std::unordered_map<std::string, std::unique_ptr<CmdDescriptionFlags>> cmddescripts;

// 函数名：setCommandDescribeEx
// 功能：设置一个全局指令说明
// 参数个数：2个
// 参数类型：字符串，字符串，整型，整型，整型
// 参数详解：cmd - 命令，description - 命令说明，level - 执行要求等级，flag1 - 命令类型1， flag2 - 命令类型2
// 备注：延期注册的情况，可能不会改变客户端界面
void setCommandDescribeEx(const char* cmd, const char* description, char level, char flag1, char flag2) {
	auto strcmd = GBKToUTF8(cmd);
	if (strcmd.length()) {
		auto flgs = std::make_unique<CmdDescriptionFlags>();
		flgs->description = GBKToUTF8(description);
		flgs->level = level;
		flgs->flag1 = flag1;
		flgs->flag2 = flag2;
		cmddescripts[strcmd] = std::move(flgs);
		if (regHandle) {
			std::string c = strcmd;
			auto ct = GBKToUTF8(description);
			SYMCALL(VA, MSSYM_MD5_8574de98358ff66b5a913417f44dd706, regHandle, &c, ct.c_str(), level, flag1, flag2);
		}
	}
}

// 函数名：runcmd
// 功能：执行后台指令
// 参数个数：1个
// 参数类型：字符串
// 参数详解：cmd - 语法正确的MC指令
// 返回值：是否正常执行
bool runcmd(const char* cmd) {
	auto strcmd = GBKToUTF8(cmd);
	if (p_spscqueue != 0) {
		if (p_level) {
			auto fr = [strcmd]() {
				SYMCALL(bool, MSSYM_MD5_b5c9e566146b3136e6fb37f0c080d91e, p_spscqueue, strcmd);
			};
			safeTick(fr);
			return true;
		}
	}
	return false;
}

// 标准输出句柄常量
static const VA STD_COUT_HANDLE = SYM_OBJECT(VA,
	MSSYM_B2UUA3impB2UQA4coutB1AA3stdB2AAA23VB2QDA5basicB1UA7ostreamB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB3AAAA11B1AA1A);

// 函数名：logout
// 功能：发送一条命令输出消息（可被拦截）
// 参数个数：1个
// 参数类型：字符串
// 参数详解：cmdout - 待发送的命令输出字符串
void logout(const char* cmdout) {
	std::string strout = GBKToUTF8(cmdout) + "\n";
	SYMCALL(VA, MSSYM_MD5_b5f2f0a753fc527db19ac8199ae8f740, STD_COUT_HANDLE, strout.c_str(), strout.length());
}

static std::unordered_map<std::string, Player*> onlinePlayers;
static std::unordered_map<Player*, bool> playerSign;

// 函数名：getOnLinePlayers
// 功能：获取在线玩家列表
// 参数个数：0个
// 返回值：玩家列表的Json字符串
std::string getOnLinePlayers() {
	Json::Value rt;
	Json::Value jv;
	mleftlock.lock();
	for (auto& op : playerSign) {
		Player* p = op.first;
		if (op.second) {
			jv["playername"] = p->getNameTag();
			jv["uuid"] = p->getUuid()->toString();
			jv["xuid"] = p->getXuid(p_level);
			jv["playerptr"] = (VA)p;
			rt.append(jv);
		}
	}
	mleftlock.unlock();
	return rt.isNull() ? "" : std::string(rt.toStyledString().c_str());
}

#if (COMMERCIAL)

// 换算两个顶点为西北下点与偏移量
static void countPos2AOff(BPos3* a1, BPos3* a2) {
	int minx, miny, minz, dx, dy, dz;
	minx = min(a1->x, a2->x);
	miny = min(a1->y, a2->y);
	minz = min(a1->z, a2->z);
	dx = abs(a1->x - a2->x);
	dy = abs(a1->y - a2->y);
	dz = abs(a1->z - a2->z);
	a1->x = minx;
	a1->y = miny;
	a1->z = minz;
	a2->x = dx + 1;
	a2->y = dy + 1;
	a2->z = dz + 1;
}

// 函数名：getStructure
// 功能：获取一个结构
// 参数个数：5个
// 参数类型：整型，字符串，字符串，布尔型，布尔型
// 参数详解：dimensionid - 地图维度，posa - 坐标JSON字符串，posb - 坐标JSON字符串，exent - 是否导出实体，exblk - 是否导出方块
// 返回值：结构json字符串
static std::string getStructure(int did, const char* jsonposa, const char* jsonposb, bool exent, bool exblk) {
	std::unique_ptr<char[]> x;
	if (p_level && (did > -1 && did < 3)) {
		Json::Value jposa = toJson(jsonposa);
		Json::Value jposb = toJson(jsonposb);
		BPos3 a, b;
		if (!jposa.isNull() && !jposb.isNull()) {
			a.x = jposa["x"].asInt();
			a.y = jposa["y"].asInt();
			a.z = jposa["z"].asInt();
			b.x = jposb["x"].asInt();
			b.y = jposb["y"].asInt();
			b.z = jposb["z"].asInt();
			countPos2AOff(&a, &b);
			VA t = StructureTemplate::getStructure(p_level, did, a, b, exent, exblk);
			std::string ret = std::string((*(Tag**)t)->toJson().toStyledString().c_str());
			(*(Tag**)t)->clearAll();
			*(VA*)t = 0;
			delete (VA*)t;
			return ret;
		}
	}
	return "";
}

// 函数名：setStructure
// 功能：设置一个结构到指定位置
// 参数个数：6个
// 参数类型：字符串，整型，字符串，整型，布尔型，布尔型
// 参数详解：strnbt - 结构JSON字符串，dimensionid - 地图维度，posa - 起始点坐标JSON字符串，rot - 旋转类型，exent - 是否导入实体，exblk - 是否导入方块
// 返回值：是否设置成功
static bool setStructure(const char* jdata, int did, const char* jsonposa, char rot, bool exent, bool exblk) {
	bool ret = false;
	if (p_level && (did > -1 && did < 3)) {
		Json::Value jposa = toJson(jsonposa);
		BPos3 a;
		if (!jposa.isNull()) {
			a.x = jposa["x"].asInt();
			a.y = jposa["y"].asInt();
			a.z = jposa["z"].asInt();
			Json::Value jv = toJson(jdata);
			VA t = Tag::fromJson(jv);
			ret = StructureTemplate::placeStructure(*(VA*)t, p_level, did, a, rot, exent, exblk);
			(*(Tag**)t)->clearAll();
			*(VA*)t = 0;
			delete (VA*)t;
		}
	}
	return ret;
}

// 获取标准能力值列表
static Json::Value getAbilities(Player* p) {
	Json::Value jv;
	for (int i = 0; i < 18; i++) {
		std::string x = Abilities::getAbilityName(i);
		if (x != "") {
			jv[x] = p->getAbility(i);
		}
	}
	return jv;
}

// 函数名：getPlayerAbilities
// 功能：获取玩家能力表
// 参数个数：1个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串
// 返回值：能力json字符串
static std::string getPlayerAbilities(const char* uuid) {
	std::string ret = "";
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		mleftlock.lock();
		auto jv = getAbilities(p);
		mleftlock.unlock();
		return std::string(jv.toStyledString().c_str());
	}
	return "";
}

// 函数名：setPlayerAbilities
// 功能：设置玩家能力表
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，newAbilities - 新能力json数据字符串
// 返回值：是否设置成功
static bool setPlayerAbilities(const char* uuid, const char* abdata) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string sabdata = abdata;
		auto fr = [suuid, sabdata]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				Json::Value jv = toJson(sabdata);
				if (jv.isNull()) {
					return;
				}
				auto members = jv.getMemberNames();
				for (auto& a : members) {
					p->setAbility(Abilities::nameToAbilityIndex(a.c_str()), jv[a].asBool());
				}
				p->setPermission(p->getPermission());
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 函数名：getPlayerAttributes
// 功能：获取玩家属性表
// 参数个数：1个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串
// 返回值：属性json字符串
static std::string getPlayerAttributes(const char* uuid) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		mleftlock.lock();
		const std::map<std::string, VA>& al = Mob::getAttrs();
		Json::Value jv;
		for (auto& k : al) {
			jv[k.first] = p->getAttr(k.first.c_str());
		}
		mleftlock.unlock();
		return std::string(jv.toStyledString().c_str());
	}
	return "";
}

// 函数名：setPlayerTempAttributes
// 功能：设置玩家属性临时值表
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，newTempAttributes - 新属性临时值json数据字符串
// 返回值：是否设置成功
// （备注：该函数可能不会变更客户端实际显示值）
static bool setPlayerTempAttributes(const char* uuid, const char* jstr) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string sjstr = jstr;
		auto fr = [suuid, sjstr]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				Json::Value jv = toJson(sjstr);
				if (jv.isNull()) {
					return;
				}
				auto members = jv.getMemberNames();
				for (auto& a : members) {
					p->setTmpAttr(a.c_str(), jv[a].asFloat());
				}
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 函数名：getPlayerMaxAttributes
// 功能：获取玩家属性上限值表
// 参数个数：1个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串
// 返回值：属性上限值json字符串
static std::string getPlayerMaxAttributes(const char* uuid) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		mleftlock.lock();
		const std::map<std::string, VA>& al = Mob::getMaxAttrs();
		Json::Value jv;
		for (auto& k : al) {
			jv[k.first] = p->getMaxAttr(k.first.c_str());
		}
		mleftlock.unlock();
		return std::string(jv.toStyledString().c_str());
	}
	return "";
}

// 函数名：setPlayerMaxAttributes
// 功能：设置玩家属性上限值表
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，newMaxAttributes - 新属性上限值json数据字符串
// 返回值：是否设置成功
// （备注：该函数可能不会变更客户端实际显示值）
static bool setPlayerMaxAttributes(const char* uuid, const char* jstr) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string sjstr = jstr;
		auto fr = [suuid, sjstr]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				Json::Value jv = toJson(sjstr);
				if (jv.isNull()) {
					return;
				}
				auto members = jv.getMemberNames();
				for (auto& a : members) {
					p->setMaxAttr(a.c_str(), jv[a].asFloat());
				}
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 函数名：getPlayerItems
// 功能：获取玩家所有物品列表
// 参数个数：1个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串
// 返回值：物品列表json字符串
static std::string getPlayerItems(const char* uuid) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		mleftlock.lock();
		Json::Value jv = p->getAllItemsList();
		mleftlock.unlock();
		return std::string(jv.toStyledString().c_str());
	}
	return "";
}

// 函数名：setPlayerItems
// 功能：设置玩家所有物品列表
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，newItems - 新物品列表json数据字符串
// 返回值：是否设置成功
// （备注：特定条件下可能不会变更游戏内实际物品）
static bool setPlayerItems(const char* uuid, const char* jstr) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string sjstr = GBKToUTF8(jstr);
		auto fr = [suuid, sjstr]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				Json::Value jv = toJson(sjstr);
				if (jv.isNull()) {
					return;
				}
				p->setAllItemsList(jv);
				p->updateInventory();
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 函数名：getPlayerSelectedItem
// 功能：获取玩家当前选中项信息
// 参数个数：1个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串
// 返回值：当前选中项信息json字符串
static std::string getPlayerSelectedItem(const char* uuid) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		mleftlock.lock();
		Json::Value jv;
		jv["selectedslot"] = p->getSelectdItemSlot();
		ItemStack* its = (ItemStack*)p->getSelectedItem();
		if (its != SYM_POINT(ItemStack, MSSYM_B1QA5EMPTYB1UA4ITEMB1AA9ItemStackB2AAA32V1B1AA1B)) {
			jv["selecteditem"] = its->toJson();
		}
		mleftlock.unlock();
		return std::string(jv.toStyledString().c_str());
	}
	return "";
}

// 函数名：addPlayerItemEx
// 功能：增加玩家一个物品
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，item - 物品json数据字符串
// 返回值：是否添加成功
// （备注：特定条件下可能不会变更游戏内实际物品）
static bool addPlayerItemEx(const char* uuid, const char* item) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string sitem = GBKToUTF8(item);
		auto fr = [suuid, sitem]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				Json::Value jv = toJson(sitem);
				if (jv.isNull()) {
					return;
				}
				ItemStack x;
				x.fromJson(jv);
				p->addItem((VA)&x);
				p->updateInventory();
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 函数名：getPlayerEffects
// 功能：获取玩家所有效果列表
// 参数个数：1个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串
// 返回值：效果列表json字符串
static std::string getPlayerEffects(const char* uuid) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		Json::Value jv = p->getAllEffects();
		if (!jv.isNull()) {
			return std::string(jv.toStyledString().c_str());
		}
	}
	return "";
}

// 函数名：setPlayerEffects
// 功能：设置玩家所有效果列表
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，newEffects - 新效果列表json数据字符串
// 返回值：是否设置成功
// （备注：特定条件下可能不会变更游戏内实际物品）
static bool setPlayerEffects(const char* uuid, const char* effs) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string seffs = effs;
		auto fr = [suuid, seffs]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				Json::Value jv = toJson(seffs);
				if (jv.isNull()) {
					return;
				}
				p->setAllEffects(jv);
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 卸载bossbar
static bool releaseBossBar(Player* p) {
	if (p) {
		VA t = 0;
		BossEventPacket sec;
		SYMCALL(VA, MSSYM_B1QE12createPacketB1AE16MinecraftPacketsB2AAA2SAB1QA2AVB2QDA6sharedB1UA3ptrB1AA7VPacketB3AAAA3stdB2AAE20W4MinecraftPacketIdsB3AAAA1Z,
			&t, 74);
		if (t) {
			BossEventPacket* pt = (BossEventPacket*)t;
			pt->mEventType = 2;	// remove player
			pt->mHealthPercent = 0;
			pt->mBossID = pt->mPlayerID = *(p->getUniqueID());
			p->sendPacket(t);
			return true;
		}
	}
	return false;
}

// 函数名：setPlayerBossBar
// 功能：设置玩家自定义血条
// 参数个数：3个
// 参数类型：字符串，字符串，浮点型
// 参数详解：uuid - 在线玩家的uuid字符串，title - 血条标题，percent - 血条百分比
// 返回值：是否设置成功
static bool setPlayerBossBar(const char* uuid, const char* title, float percent) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string stitle = GBKToUTF8(title);
		auto fr = [suuid, stitle, percent]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				if (releaseBossBar(p)) {
					VA t = 0;
					BossEventPacket sec;
					SYMCALL(VA, MSSYM_B1QE12createPacketB1AE16MinecraftPacketsB2AAA2SAB1QA2AVB2QDA6sharedB1UA3ptrB1AA7VPacketB3AAAA3stdB2AAE20W4MinecraftPacketIdsB3AAAA1Z,
						&t, 74);
					if (t) {
						BossEventPacket* pt = (BossEventPacket*)t;
						pt->mName = stitle;
						pt->mEventType = 0;	// add player
						pt->mHealthPercent = percent;
						pt->mBossID = pt->mPlayerID = *(p->getUniqueID());
						p->sendPacket(t);
					}
				}
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 函数名：removePlayerBossBar
// 功能：清除玩家自定义血条
// 参数个数：1个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串
// 返回值：是否清除成功
static bool removePlayerBossBar(const char* uuid) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		auto fr = [suuid]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				releaseBossBar(p);
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}


// 函数名：teleport
// 功能：传送玩家至指定坐标和维度
// 参数个数：5个
// 参数类型：字符串，浮点型，浮点型，浮点型，整型
// 参数详解：uuid - 在线玩家的uuid字符串，X - x，Y - y，Z - z，dimensionid - 维度ID
// 返回值：是否传送成功
static bool teleport(const char* uuid, float x, float y, float z, int did) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		auto fr = [suuid, x, y, z, did]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				Vec3 v;
				v.x = x;
				v.y = y;
				v.z = z;
				p->teleport(&v, did);
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}


// 函数名：setPlayerSidebar
// 功能：设置玩家自定义侧边栏临时计分板
// 参数个数：3个
// 参数类型：字符串，字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，title - 侧边栏标题，list - 列表字符串数组
// 返回值：是否设置成功
// （注：列表总是从第1行开始，总计不超过15行）
static bool setPlayerSidebar(const char* uuid, const char* title, const char* list) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string stitle = GBKToUTF8(title);
		std::string slist = GBKToUTF8(list);
		auto fr = [suuid, stitle, slist]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				Json::Value jv = toJson(slist);
				std::vector<std::string> l;
				if (!jv.isNull()) {
					for (auto& v : jv) {
						l.push_back(v.asString());
					}
				}
				((Scoreboard*)((Level*)p->getLevel())->getScoreBoard())->sendCustemData(p, stitle, l);
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 函数名：removePlayerSidebar
// 功能：清除玩家自定义侧边栏
// 参数个数：1个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串
// 返回值：是否清除成功
static bool removePlayerSidebar(const char* uuid) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		auto fr = [suuid]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				((Scoreboard*)((Level*)p->getLevel())->getScoreBoard())->removeCustemScoreData(p);
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 函数名：getPlayerPermissionAndGametype
// 功能：获取玩家权限与游戏模式
// 参数个数：1个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串
// 返回值：权限与模式的json字符串
static std::string getPlayerPermissionAndGametype(const char* uuid) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		mleftlock.lock();
		Json::Value jv;
		jv["oplevel"] = (int)p->getPermission();
		jv["permission"] = (int)p->getPermissionLevel();
		jv["gametype"] = p->getGameType();
		mleftlock.unlock();
		return std::string(jv.toStyledString().c_str());
	}
	return "";
}

// 函数名：setPlayerPermissionAndGametype
// 功能：设置玩家权限与游戏模式
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，newModes - 新权限或模式json数据字符串
// 返回值：是否设置成功
// （备注：特定条件下可能不会变更游戏内实际能力）
static bool setPlayerPermissionAndGametype(const char* uuid, const char* newModes) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string jvdata = newModes;
		auto fr = [suuid, jvdata]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				Json::Value jv = toJson(jvdata);
				if (!jv.isNull()) {
					Json::Value ol = jv["oplevel"];
					Json::Value per = jv["permission"];
					Json::Value ty = jv["gametype"];
					if (!ol.isNull()) {
						p->setPermission((char)ol.asInt());
					}
					if (!per.isNull()) {
						p->setPermissionLevel((char)per.asInt());
						p->setPermission(p->getPermission());
					}
					if (!ty.isNull()) {
						p->setGameType(ty.asInt());
					}
				}
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}


#endif

// 函数名：reNameByUuid
// 功能：重命名一个指定的玩家名
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，newName - 新的名称
// 返回值：是否命名成功
// （备注：该函数可能不会变更客户端实际显示名）
bool reNameByUuid(const char* cuuid, const char* cnewName) {
	bool ret = false;
	std::string uuid = std::string(cuuid);
	std::string newName = GBKToUTF8(cnewName);
	Player* taget = onlinePlayers[uuid];
	if (playerSign[taget]) {
		auto fr = [uuid, newName]() {
			Player* p = onlinePlayers[uuid];
			if (playerSign[p]) {
				p->reName(newName);
			}
		};
		safeTick(fr);
		ret = true;
	}
	return ret;
}

// 函数名：addPlayerItem
// 功能：增加玩家一个物品
// 参数个数：4个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串，id - 物品id值，aux - 物品特殊值，count - 数量
// 返回值：是否增加成功
// （备注：特定条件下可能不会变更游戏内实际物品）
bool addPlayerItem(const char* uuid, int id, short aux, char count) {
	Player* p = onlinePlayers[uuid];
	bool ret = false;
	if (playerSign[p]) {
		std::string suuid = uuid;
		auto fr = [suuid, id, aux, count]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				ItemStack x;
				x.getFromId(id, aux, count);
				p->addItem((VA)&x);
				p->updateInventory();
			}
		};
		safeTick(fr);
		ret = true;
	}
	return ret;
}


// 在JSON数据中附上玩家基本信息
static void addPlayerJsonInfo(Json::Value& jv, Player* p) {
	if (p) {
		jv["playername"] = p->getNameTag();
		int did = p->getDimensionId();
		jv["dimensionid"] = did;
		jv["dimension"] = toDimenStr(did);
		jv["isstand"] = p->isStand();
		jv["XYZ"] = toJson(p->getPos()->toJsonString());
		jv["playerptr"] = (VA)p;
	}
}

// 函数名：selectPlayer
// 功能：查询在线玩家基本信息
// 参数个数：1个
// 参数类型：字符串
// 参数详解：uuid - 在线玩家的uuid字符串
// 返回值：玩家基本信息json字符串
std::string selectPlayer(const char* uuid) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		mleftlock.lock();
		Json::Value jv;
		addPlayerJsonInfo(jv, p);
		jv["uuid"] = p->getUuid()->toString();
		jv["xuid"] = p->getXuid(p_level);
#if (COMMERCIAL)
		jv["health"] = p->getAttr("health");
#endif
		mleftlock.unlock();
		return std::string(jv.toStyledString().c_str());
	}
	return "";
}

// 函数名：talkAs
// 功能：模拟玩家发送一个文本
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，msg - 待模拟发送的文本
// 返回值：是否发送成功
bool talkAs(const char* uuid, const char* msg) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {								// IDA ServerNetworkHandler::handle, https://github.com/NiclasOlofsson/MiNET/blob/master/src/MiNET/MiNET/Net/MCPE%20Protocol%20Documentation.md
		std::string suuid = uuid;
		std::string txt = GBKToUTF8(msg);
		auto fr = [suuid, txt]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				std::string n = p->getNameTag();
				VA nid = p->getNetId();
				VA tpk;
				TextPacket sec;
				SYMCALL(VA, MSSYM_B1QE12createPacketB1AE16MinecraftPacketsB2AAA2SAB1QA2AVB2QDA6sharedB1UA3ptrB1AA7VPacketB3AAAA3stdB2AAE20W4MinecraftPacketIdsB3AAAA1Z,
					&tpk, 9);
				*(char*)(tpk + 40) = 1;
				memcpy((void*)(tpk + 48), &n, sizeof(n));
				memcpy((void*)(tpk + 80), &txt, sizeof(txt));
				SYMCALL(VA, MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE14AEBVTextPacketB3AAAA1Z,
					p_ServerNetworkHandle, nid, tpk);
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 函数名：runcmdAs
// 功能：模拟玩家执行一个指令
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，cmd - 待模拟执行的指令
// 返回值：是否发送成功
bool runcmdAs(const char* uuid, const char* cmd) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string scmd = GBKToUTF8(cmd);
		auto fr = [suuid, scmd]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				VA nid = p->getNetId();
				VA tpk;
				CommandRequestPacket src;
				SYMCALL(VA, MSSYM_B1QE12createPacketB1AE16MinecraftPacketsB2AAA2SAB1QA2AVB2QDA6sharedB1UA3ptrB1AA7VPacketB3AAAA3stdB2AAE20W4MinecraftPacketIdsB3AAAA1Z,
					&tpk, 76);
				memcpy((void*)(tpk + 40), &scmd, sizeof(scmd));
				SYMCALL(VA, MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE24AEBVCommandRequestPacketB3AAAA1Z,
					p_ServerNetworkHandle, nid, tpk);
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 函数名：disconnectClient
// 功能：断开一个玩家的连接
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，tips - 断开提示（设空值则为默认值）
// 返回值：是否发送成功
bool disconnectClient(const char* uuid, const char* tips) {
	Player* p = onlinePlayers[uuid];
	if (playerSign[p]) {
		std::string suuid = uuid;
		std::string stips = "disconnectionScreen.disconnected";
		if (tips) {
			auto s = GBKToUTF8(tips);
			if (s.length())
				stips = s;
		}
		auto fr = [suuid, stips]() {
			Player* p = onlinePlayers[suuid];
			if (playerSign[p]) {
				VA nid = p->getNetId();
				SYMCALL(VA, MSSYM_MD5_389e602d185eac21ddcc53a5bb0046ee,
					p_ServerNetworkHandle, nid, 0, stips, 0);
			}
		};
		safeTick(fr);
		return true;
	}
	return false;
}

// 判断指针是否为玩家列表中指针
static bool checkIsPlayer(void* p) {
	return playerSign[(Player*)p];
}

static std::map<unsigned, bool> fids;

// 获取一个未被使用的基于时间秒数的id
static unsigned getFormId() {
	unsigned id = (unsigned)(time(0) + rand());
	do {
		++id;
	} while (id == 0 || fids[id]);
	fids[id] = true;
	return id;
}

// 函数名：releaseForm
// 功能：放弃一个表单
// 参数个数：1个
// 参数类型：整型
// 参数详解：formid - 表单id
// 返回值：是否释放成功
//（备注：已被接收到的表单会被自动释放）
bool releaseForm(unsigned fid)
{
	if (fids[fid]) {
		fids.erase(fid);
		return true;
	}
	return false;
}

// 发送一个SimpleForm的表单数据包
static unsigned sendForm(std::string uuid, std::string str)
{
	unsigned fid = getFormId();
	// 此处自主创建包
	auto fr = [uuid, fid, str]() {
		Player* p = onlinePlayers[uuid];
		if (playerSign[p]) {
			VA tpk;
			ModalFormRequestPacket sec;
			SYMCALL(VA, MSSYM_B1QE12createPacketB1AE16MinecraftPacketsB2AAA2SAB1QA2AVB2QDA6sharedB1UA3ptrB1AA7VPacketB3AAAA3stdB2AAE20W4MinecraftPacketIdsB3AAAA1Z,
				&tpk, 100);
			*(VA*)(tpk + 40) = fid;
			*(std::string*)(tpk + 48) = str;
			p->sendPacket(tpk);
		}
	};
	safeTick(fr);
	return fid;
}

// 函数名：sendSimpleForm
// 功能：向指定的玩家发送一个简单表单
// 参数个数：4个
// 参数类型：字符串，字符串，字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，title - 表单标题，content - 内容，buttons - 按钮文本数组字符串
// 返回值：创建的表单id，为 0 表示发送失败
UINT sendSimpleForm(char* uuid, char* title, char* content, char* buttons) {
	Player* p = onlinePlayers[uuid];
	if (!playerSign[p])
		return 0;
	auto stitle = GBKToUTF8(title);
	auto scontent = GBKToUTF8(content);
	auto sbuttons = GBKToUTF8(buttons);
	Json::Value bts;
	Json::Value ja = toJson(sbuttons);
	for (unsigned i = 0; i < ja.size(); i++) {
		Json::Value bt;
		bt["text"] = ja[i];
		bts.append(bt);
	}
	if (bts.isNull())
		bts = toJson("[]");
	std::string str = createSimpleFormString(stitle, scontent, bts);
	return sendForm(uuid, str);
}

// 函数名：sendModalForm
// 功能：向指定的玩家发送一个模式对话框
// 参数个数：5个
// 参数类型：字符串，字符串，字符串，字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，title - 表单标题，content - 内容，button1 按钮1标题（点击该按钮selected为true），button2 按钮2标题（点击该按钮selected为false）
// 返回值：创建的表单id，为 0 表示发送失败
UINT sendModalForm(char* uuid, char* title, char* content, char* button1, char* button2) {
	Player* p = onlinePlayers[uuid];
	if (!playerSign[p])
		return 0;
	auto utitle = GBKToUTF8(title);
	auto ucontent = GBKToUTF8(content);
	auto ubutton1 = GBKToUTF8(button1);
	auto ubutton2 = GBKToUTF8(button2);
	std::string str = createModalFormString(utitle, ucontent, ubutton1, ubutton2);
	return sendForm(uuid, str);
}

// 函数名：sendCustomForm
// 功能：向指定的玩家发送一个自定义表单
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，json - 自定义表单的json字符串（要使用自定义表单类型，参考nk、pm格式或minebbs专栏）
// 返回值：创建的表单id，为 0 表示发送失败
UINT sendCustomForm(char* uuid, char* json) {
	Player* p = onlinePlayers[uuid];
	if (!playerSign[p])
		return 0;
	auto ujson = GBKToUTF8(json);
	return sendForm(uuid, ujson);
}

#if (MODULE_05007)

// 函数名：getscorebroardValue
// 功能：获取指定玩家指定计分板上的数值
// 参数个数：2个
// 参数类型：字符串，字符串
// 参数详解：uuid - 在线玩家的uuid字符串，objname - 计分板登记的名称
// 返回值：获取的目标值，若目标不存在则发信一条创建指令
int getscoreboardValue(const char* uuid, const char* objname) {
	Player* p = onlinePlayers[uuid];
	if (!playerSign[p])
		return 0;
	auto oname = GBKToUTF8(objname);
	return getscoreboard(p, oname);
}

#endif

// 附加玩家信息
static void addPlayerInfo(PlayerEvent* pe, Player* p) {
	autoByteCpy(&pe->playername, p->getNameTag().c_str());
	pe->dimensionid = p->getDimensionId();
	autoByteCpy(&pe->dimension, toDimenStr(pe->dimensionid).c_str());
	memcpy(&pe->XYZ, p->getPos(), sizeof(Vec3));
	pe->isstand = p->isStand();
}

static void addPlayerDieInfo(MobDieEvent* ue, Player* pPlayer) {
	autoByteCpy(&ue->playername, pPlayer->getNameTag().c_str());
	ue->dimensionid = pPlayer->getDimensionId();
	autoByteCpy(&ue->dimension, toDimenStr(ue->dimensionid).c_str());
	memcpy(&ue->XYZ, pPlayer->getPos(), sizeof(Vec3));
	ue->isstand = pPlayer->isStand();
}

static void addMobDieInfo(MobDieEvent* ue, Mob* p) {
	autoByteCpy(&ue->mobname, p->getNameTag().c_str());
	int did = p->getDimensionId();
	ue->dimensionid = did;
	memcpy(&ue->XYZ, p->getPos(), sizeof(Vec3));
}

// 回传伤害源信息
static void getDamageInfo(void* p, void* dsrc, MobDieEvent* ue) {			// IDA Mob::die
	char v72;
	VA  v2[2];
	v2[0] = (VA)p;
	v2[1] = (VA)dsrc;
	auto v7 = *((VA*)(v2[0] + 816));
	auto srActid = (VA*)(*(VA(__fastcall**)(VA, char*))(*(VA*)v2[1] + 64))(
		v2[1], &v72);
	auto SrAct = SYMCALL(Actor*,
		MSSYM_B1QE11fetchEntityB1AA5LevelB2AAE13QEBAPEAVActorB2AAE14UActorUniqueIDB3AAUA1NB1AA1Z,
		v7, *srActid, 0);
	std::string sr_name = "";
	std::string sr_type = "";
	if (SrAct) {
		sr_name = SrAct->getNameTag();
		int srtype = checkIsPlayer(SrAct) ? 319 : SrAct->getEntityTypeId();
		SYMCALL(std::string&, MSSYM_MD5_af48b8a1869a49a3fb9a4c12f48d5a68, &sr_type, srtype);
	}
	Json::Value jv;
	if (checkIsPlayer(p)) {
		addPlayerDieInfo(ue, (Player*)p);
		std::string playertype;				// IDA Player::getEntityTypeId
		SYMCALL(std::string&, MSSYM_MD5_af48b8a1869a49a3fb9a4c12f48d5a68, &playertype, 319);
		autoByteCpy(&ue->mobname, ue->playername);
		autoByteCpy(&ue->mobtype, playertype.c_str());	// "entity.player.name"
	}
	else {
		addMobDieInfo(ue, (Mob*)p);
		autoByteCpy(&ue->mobtype, ((Mob*)p)->getEntityTypeName().c_str());
	}
	autoByteCpy(&ue->srcname, sr_name.c_str());
	autoByteCpy(&ue->srctype, sr_type.c_str());
	ue->dmcase = *((int*)dsrc + 2);
}

//////////////////////////////// 组件 API 区域 ////////////////////////////////

#pragma region 类属性相关方法及实现

std::string Actor::sgetArmorContainer(Actor* e) {
	VA parmor = ((Mob*)e)->getArmor();
	std::vector<ItemStack*> armors;
	(*(void(**)(VA, VA))(**(VA**)parmor + 152))(*(VA*)parmor, (VA)&armors);		// IDA Mob::addAdditionalSaveData
	Json::Value jv;
	if (armors.size() > 0) {
		for (size_t i = 0, l = armors.size(); i < l; i++) {
			auto pa = armors[i];
			Json::Value ji;
			ji["Slot"] = i;
			ji["item"] = pa->getName();
			ji["id"] = pa->getId();
			ji["count"] = pa->mCount;
			jv.append(ji);
		}
		return jv.toStyledString();
	}
	return "";
}

std::string Actor::sgetAttack(Actor* e) {
	if (*(VA*)(((VA*)e)[28] + 712)) {	// IDA ScriptAttackComponent::retrieveComponentFrom
		VA atattr = (*(VA(__fastcall**)(Actor*, VA*))(*(VA*)e + 1536))(
			e,
			SYM_POINT(VA, MSSYM_B1QA6ATTACKB1UA6DAMAGEB1AE16SharedAttributesB2AAE112VAttributeB2AAA1B));
		if (*(VA*)(atattr + 16)) {
			float dmin = *(float*)(atattr + 124);
			float dmax = *(float*)(atattr + 128);
			Json::Value jv;
			jv["range_min"] = dmin;
			jv["range_max"] = dmax;
			return jv.toStyledString();
		}
	}
	return "";
}
bool Actor::ssetAttack(Actor* e, const char* damage) {
	std::string dmg = damage;
	Json::Value jv = toJson(damage);
	if (!jv.isNull()) {
		float dmin = jv["range_min"].asFloat();
		float dmax = jv["range_max"].asFloat();
		VA mptr = ((VA*)e)[135];					// IDA ScriptAttackComponent::applyComponentTo
		if (mptr) {
			VA v20 = SYMCALL(VA, MSSYM_B1QE17registerAttributeB1AE16BaseAttributeMapB2AAE25QEAAAEAVAttributeInstanceB2AAE13AEBVAttributeB3AAAA1Z,
				mptr, SYM_POINT(VA, MSSYM_B1QA6ATTACKB1UA6DAMAGEB1AE16SharedAttributesB2AAE112VAttributeB2AAA1B));
			SYMCALL(VA, MSSYM_B1QA8setRangeB1AE17AttributeInstanceB2AAA8QEAAXMMMB1AA1Z, v20, dmin, dmin, dmax);
			VA v22 = SYMCALL(VA, MSSYM_B1QE17registerAttributeB1AE16BaseAttributeMapB2AAE25QEAAAEAVAttributeInstanceB2AAE13AEBVAttributeB3AAAA1Z,
				mptr, SYM_POINT(VA, MSSYM_B1QA6ATTACKB1UA6DAMAGEB1AE16SharedAttributesB2AAE112VAttributeB2AAA1B));
			SYMCALL(VA, MSSYM_B1QE19resetToDefaultValueB1AE17AttributeInstanceB2AAA7QEAAXXZ, v22);
			return true;
		}
	}
	return false;
}

std::string Actor::sgetCollisionBox(Actor* e) {
	float w = *((float*)e + 285);		// IDA Actor::_refreshAABB
	float h = *((float*)e + 286);
	Json::Value jv;
	jv["width"] = w;
	jv["height"] = h;
	return jv.toStyledString();
}
bool Actor::ssetCollisionBox(Actor* e, const char* box) {
	std::string sbox = box;
	Json::Value jv = toJson(sbox);
	if (!jv.isNull()) {
		float w = jv["width"].asFloat();
		float h = jv["height"].asFloat();
		if (w != 0 && h != 0) {			// IDA SynchedActorData::set<float>
			*((float*)e + 285) = w;
			*((float*)e + 286) = h;
			SYMCALL(VA, MSSYM_B1QA7setSizeB1AA5ActorB2AAA7UEAAXMMB1AA1Z, e, w, h);
			SYMCALL(VA, MSSYM_B3QQDA3setB1AA1MB1AE16SynchedActorDataB2AAE10QEAAXGAEBMB1AA1Z, (VA)e + 320, ActorDataIDs::WIDTH, &w);
			SYMCALL(VA, MSSYM_B3QQDA3setB1AA1MB1AE16SynchedActorDataB2AAE10QEAAXGAEBMB1AA1Z, (VA)e + 320, ActorDataIDs::HEIGHT, &h);
			return true;
		}
	}
	return false;
}

std::string Actor::sgetHandContainer(Actor* e) {
	VA phand = ((Mob*)e)->getHands();
	if (phand) {
		Json::Value jv;
		std::vector<ItemStack*> hands;		// IDA ScriptHandContainerComponent::retrieveComponentFrom
		(ItemStack*)(*(VA(__fastcall**)(VA, VA))(**(VA**)phand + 152))(
			*(VA*)phand, (VA)&hands);
		if (Actor::sgetEntityTypeId(e) == 319) {
			ItemStack* v6 = (*(ItemStack *(__fastcall**)(Actor*))(*(VA*)e + 1200))(e);
			hands[0] = v6;
		}
		for (VA i = 0, l = hands.size(); i < l; i++) {
			ItemStack* h = hands[i];
			Json::Value ji;
			ji["Slot"] = i;
			ji["item"] = h->getName();
			ji["id"] = h->getId();
			ji["count"] = h->mCount;
			jv.append(ji);
		}
		return jv.toStyledString();
	}
	return "";
}

std::string Actor::sgetHealth(Actor* e) {
	VA bpattrmap = ((VA*)e)[135];			// IDA ScriptHealthComponent::retrieveComponentFrom
	if (bpattrmap) {
		VA hattr = SYMCALL(VA, MSSYM_B1QE18getMutableInstanceB1AE16BaseAttributeMapB2AAE25QEAAPEAVAttributeInstanceB2AAA1IB1AA1Z,
			bpattrmap, SYM_OBJECT(UINT32, MSSYM_B1QA6HEALTHB1AE16SharedAttributesB2AAE112VAttributeB2AAA1B + 4));
		if (hattr) {
			float value = *((float*)hattr + 33);
			float max = *((float*)hattr + 32);
			Json::Value jv;
			jv["value"] = value;
			jv["max"] = max;
			return jv.toStyledString();
		}
	}
	return "";
}
bool Actor::ssetHealth(Actor* e, const char* hel) {
	std::string shel = hel;
	Json::Value jv = toJson(shel);
	if (!jv.isNull()) {
		float value = jv["value"].asFloat();
		float max = jv["max"].asFloat();
		VA bpattrmap = ((VA*)e)[135];
		if (bpattrmap) {
			VA hattr = SYMCALL(VA, MSSYM_B1QE18getMutableInstanceB1AE16BaseAttributeMapB2AAE25QEAAPEAVAttributeInstanceB2AAA1IB1AA1Z,
				bpattrmap, SYM_OBJECT(UINT32, MSSYM_B1QA6HEALTHB1AE16SharedAttributesB2AAE112VAttributeB2AAA1B + 4));
			if (hattr) {
				*((float*)hattr + 33) = value;
				*((float*)hattr + 32) = max;
				SYMCALL(VA, MSSYM_B2QUA8setDirtyB1AE17AttributeInstanceB2AAA7AEAAXXZ, hattr);
				return true;
			}
		}
	}
	return false;
}

std::string Actor::sgetInventoryContainer(Actor* e) {
	std::vector<ItemStack*> bag;		// IDA ScriptInventoryContainerComponent::retrieveComponentFrom
	if (Actor::sgetEntityTypeId(e) == 319) {	// 玩家
		VA cont = ((Player*)e)->getSupplies();
		(*(void(__fastcall**)(VA, void*))(*(VA*)cont + 152))(
			cont,
			&bag);
	}
	else {
		VA v8 = SYMCALL(VA, MSSYM_B3QQDE15tryGetComponentB1AE19VContainerComponentB3AAAA5ActorB2AAE26QEAAPEAVContainerComponentB2AAA2XZ, e);
		if (v8) {
			(*(void(__fastcall**)(VA, void*))(**(VA**)(v8 + 8) + 152))(*(VA*)(v8 + 8), &bag);
		}
	}
	size_t l = bag.size();
	if (l > 0) {
		Json::Value jv;
		for (size_t i = 0; i < l; i++) {
			ItemStack* h = bag[i];
			Json::Value ji;
			ji["Slot"] = i;
			ji["item"] = h->getName();
			ji["id"] = h->getId();
			ji["count"] = h->mCount;
			jv.append(ji);
		}
		return jv.toStyledString();
	}
	return "";
}

std::string Actor::sgetName(Actor* e) {
	return e->getNameTag();
}
bool Actor::ssetName(Actor* e, const char* n, bool alwaysShow) {
	std::string nname = GBKToUTF8(n);
	if (Actor::sgetEntityTypeId(e) == 319) {
		((Player*)e)->reName(nname);
	} else
		SYMCALL(VA, MSSYM_MD5_2f9772d3549cbbfca05bc883e3dd5c30, e, nname);
	bool v = alwaysShow;					// IDA SynchedActorData::set<signed char>
	SYMCALL(VA, MSSYM_B3QQDA3setB1AA1CB1AE16SynchedActorDataB2AAE10QEAAXGAEBCB1AA1Z, (VA)e + 320, ActorDataIDs::NAMETAG_ALWAYS_SHOW, &v);
	return true;
}

std::string Actor::sgetPosition(Actor* e) {
	auto pos = e->getPos();
	Json::Value jv;
	if (pos) {
		jv["x"] = pos->x;
		jv["y"] = pos->y;
		jv["z"] = pos->z;
		return jv.toStyledString();
	}
	return "";
}

bool Actor::ssetPosition(Actor* e, const char* jpos) {
	Json::Value jv = toJson(jpos);
	if (!jv.isNull()) {
		Vec3 v;
		v.x = jv["x"].asFloat();
		v.y = jv["y"].asFloat();
		v.z = jv["z"].asFloat();
		int v9 = (int)ActorType::Undefined_2;		// IDA ScriptPositionComponent::applyComponentTo
		(*(void(__fastcall**)(Actor*, Vec3*, VA, VA, signed int, VA*))(*(VA*)e + 264))(
			e, &v, 1, 0, v9, SYM_POINT(VA, MSSYM_B1QA7INVALIDB1UA2IDB1AE13ActorUniqueIDB2AAA32U1B1AA1B));
		if (Actor::sgetEntityTypeId(e) == 319) {
			v9 = (int)ActorType::Player_0;
			SYMCALL(VA, MSSYM_B1QE10teleportToB1AA6PlayerB2AAE13UEAAXAEBVVec3B3AAUE20NHHAEBUActorUniqueIDB3AAAA1Z, e, &v, 1, 0,
				v9, e->getUniqueID());
		}
		return true;
	}
	return false;
}

std::string Actor::sgetRotation(Actor* e) {
	Json::Value jv;
	float x = ((float*)e)[64];					// IDA Actor::setRot
	float y = ((float*)e)[65];
	jv["x"] = x;
	jv["y"] = y;
	return jv.toStyledString();
}

bool Actor::ssetRotation(Actor* e, const char* jr) {
	Json::Value jv = toJson(jr);
	if (!jv.isNull()) {
		float x = jv["x"].asFloat();
		float y = jv["y"].asFloat();
		Vec2 v2;
		v2.x = x;
		v2.y = y;
		SYMCALL(VA, MSSYM_B1QA6setRotB1AA5ActorB2AAE13UEAAXAEBVVec2B3AAAA1Z, e, &v2);
		if (Actor::sgetEntityTypeId(e) == 319) {
			Vec3 c3;
			memcpy(&c3, e->getPos(), sizeof(Vec3));
			float h = *((float*)e + 286);				// IDA Actor::_refreshAABB
			c3.y -= (float)(1.7999999523162842 * 0.9);
			auto v9 = (int)ActorType::Player_0;
			SYMCALL(VA, MSSYM_B1QE10teleportToB1AA6PlayerB2AAE13UEAAXAEBVVec3B3AAUE20NHHAEBUActorUniqueIDB3AAAA1Z, e, &c3, 1, 0,
				v9, e->getUniqueID());
		}
		return true;
	}
	return false;
}

int Actor::sgetDimensionId(Actor* e) {
	return e->getDimensionId();
}

int Actor::sgetEntityTypeId(Actor* e) {
	return (*(int(__fastcall**)(Actor*))(*(VA*)e + 1272))(e);	// IDA ScriptPositionComponent::applyComponentTo
}

VA Actor::sgetUniqueID(Actor* e) {
	return *e->getUniqueID();
}

bool Actor::sremove(Actor* e) {
	(*(void(**)(Actor*))(*(VA*)e + 88))(e);			// IDA Actor::remove , check vtable
	return *((char*)e + 905) == 1;
}

Actor* Actor::sgetfromUniqueID(VA id) {
	if (p_level)
		return SYMCALL(Actor*, MSSYM_B1QE11fetchEntityB1AA5LevelB2AAE13QEBAPEAVActorB2AAE14UActorUniqueIDB3AAUA1NB1AA1Z, p_level, id, 0);
	return NULL;
}

std::vector<VA*>* Actor::sgetEntities(int did, float x1, float y1, float z1, float x2, float y2, float z2) {
	if (p_level) {
		if (did > -1 && did < 3) {
			AABB rt;
			rt.set(x1, y1, z1, x2, y2, z2);
			BlockSource* bs = (BlockSource*)((Level*)p_level)->getDimension(did)->getBlockSource();
			return bs->getEntities((VA*)&rt);
		}
	}
	return 0;
}

// Player相关

std::string Player::sgetHotbarContainer(Player* p) {
	VA cont = p->getSupplies();
	if (cont) {
		std::vector<ItemStack*> its;		// IDA ScriptHotbarContainerComponent::retrieveComponentFrom
		(*(void(__fastcall**)(VA, void*))(*(VA*)cont + 152))(
			cont,
			&its);
		if (its.size() >= 9) {
			Json::Value jv;
			for (int i = 0; i < 9; i++) {
				ItemStack* h = its[i];
				Json::Value ji;
				ji["Slot"] = i;
				ji["item"] = h->getName();
				ji["id"] = h->getId();
				ji["count"] = h->mCount;
				jv.append(ji);
			}
			return jv.toStyledString();
		}
	}
	return "";
}

std::string Player::sgetUuid(Player* p) {
	return p->getUuid()->toString();
}

// 类属性相关方法
static struct McMethods {
	std::unordered_map<std::string, void*> mcMethods;
public:
	// 初始化，装入所有类成员属性
	McMethods() {
		MCMETHOD m;
		mcMethods[m.ENTITY_GET_ARMOR_CONTAINER] = &Actor::sgetArmorContainer;
		mcMethods[m.ENTITY_GET_ATTACK] = &Actor::sgetAttack;
		mcMethods[m.ENTITY_SET_ATTACK] = &Actor::ssetAttack;
		mcMethods[m.ENTITY_GET_COLLISION_BOX] = &Actor::sgetCollisionBox;
		mcMethods[m.ENTITY_SET_COLLISION_BOX] = &Actor::ssetCollisionBox;
		mcMethods[m.ENTITY_GET_HAND_CONTAINER] = &Actor::sgetHandContainer;
		mcMethods[m.ENTITY_GET_HEALTH] = &Actor::sgetHealth;
		mcMethods[m.ENTITY_SET_HEALTH] = &Actor::ssetHealth;
		mcMethods[m.ENTITY_GET_INVENTORY_CONTAINER] = &Actor::sgetInventoryContainer;
		mcMethods[m.ENTITY_GET_NAME] = &Actor::sgetName;
		mcMethods[m.ENTITY_SET_NAME] = &Actor::ssetName;
		mcMethods[m.ENTITY_GET_POSITION] = &Actor::sgetPosition;
		mcMethods[m.ENTITY_SET_POSITION] = &Actor::ssetPosition;
		mcMethods[m.ENTITY_GET_ROTATION] = &Actor::sgetRotation;
		mcMethods[m.ENTITY_SET_ROTATION] = &Actor::ssetRotation;
		mcMethods[m.ENTITY_GET_DIMENSIONID] = &Actor::sgetDimensionId;
		mcMethods[m.ENTITY_GET_TYPEID] = &Actor::sgetEntityTypeId;
		mcMethods[m.ENTITY_GET_UNIQUEID] = &Actor::sgetUniqueID;
		mcMethods[m.ENTITY_REMOVE] = &Actor::sremove;
		mcMethods[m.LEVEL_GETFROM_UNIQUEID] = &Actor::sgetfromUniqueID;
		mcMethods[m.LEVEL_GETSFROM_AABB] = &Actor::sgetEntities;
		mcMethods[m.PLAYER_GET_HOTBAR_CONTAINER] = &Player::sgetHotbarContainer;
		mcMethods[m.PLAYER_GET_UUID] = &Player::sgetUuid;
	}
	void* getMcMethod(std::string methodname) {
		return mcMethods[methodname];
	}
} _McMethods;

#pragma endregion

void* mcComponentAPI(const char* method) {
	return _McMethods.getMcMethod(method);
}

//////////////////////////////// 静态 HOOK 区域 ////////////////////////////////

#if 0 // 测试用途

int s = 5;

// 强制报错
static void herror() {
	PR(1 / (s - 5));
}

bool hooked = false;

#endif

// 此处开始接收异步数据			// IDA main
THook2(_CS_MAIN, VA, MSSYM_A4main,
	VA a1, VA a2, VA a3) {
	initMods();
	return original(a1, a2, a3);
}

// 获取指令队列
THook2(_CS_GETSPSCQUEUE, VA, MSSYM_MD5_3b8fb7204bf8294ee636ba7272eec000,
	VA _this) {
	p_spscqueue = original(_this);
	return p_spscqueue;
}

// 获取游戏初始化时基本信息
THook2(_CS_ONGAMESESSION, VA,
	MSSYM_MD5_9f3b3524a8d04242c33d9c188831f836,
	void* a1, void* a2, VA* a3, void* a4, void* a5, void* a6, void* a7) {
	p_ServerNetworkHandle = *a3;
	return original(a1, a2, a3, a4, a5, a6, a7);
}

// 获取地图初始化信息
THook2(_CS_LEVELINIT, VA, MSSYM_MD5_96d831b409d1a1984d6ac116f2bd4a55,
	VA a1, VA a2, VA a3, VA a4, VA a5, VA a6, VA a7, VA a8, VA a9, VA a10, VA a11, VA a12, VA a13) {
	VA level = original(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
	p_level = level;
	return level;
}

// 注册指令描述引用自list命令注册
THook2(_CS_ONLISTCMDREG, VA, MSSYM_B1QA5setupB1AE11ListCommandB2AAE22SAXAEAVCommandRegistryB3AAAA1Z,
	VA handle) {
	regHandle = handle;
	for (auto& v : cmddescripts) {
		std::string c = std::string(v.first);
		std::string ct = v.second->description;
		char level = v.second->level;
		char f1 = v.second->flag1;
		char f2 = v.second->flag2;
		SYMCALL(VA, MSSYM_MD5_8574de98358ff66b5a913417f44dd706, handle, &c, ct.c_str(), level, f1, f2);
	}
	return original(handle);
}

// 服务器后台输入指令
static bool _CS_ONSERVERCMD(VA _this, std::string* cmd) {
	Events e;
	e.type = EventType::onServerCmd;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	ServerCmdEvent se;
	autoByteCpy(&se.cmd, cmd->c_str());
	e.data = &se;
	bool ret = runCscode(ActEvent.ONSERVERCMD, ActMode::BEFORE, e);
	if (ret) {
		auto original = (bool(*)(VA, std::string*)) * getOriginalData(_CS_ONSERVERCMD);
		ret = original(_this, cmd);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONSERVERCMD, ActMode::AFTER, e);
	}
	se.releaseAll();
	return ret;
}
static VA ONSERVERCMD_SYMS[] = {1, MSSYM_MD5_b5c9e566146b3136e6fb37f0c080d91e, (VA)_CS_ONSERVERCMD };


// 服务器后台指令输出
static VA _CS_ONSERVERCMDOUTPUT(VA handle, char* str, VA size) {
	auto original = (VA(*)(VA, char*, VA)) * getOriginalData(_CS_ONSERVERCMDOUTPUT);
	if (handle == STD_COUT_HANDLE) {
		Events e;
		e.type = EventType::onServerCmdOutput;
		e.mode = ActMode::BEFORE;
		e.result = 0;
		ServerCmdOutputEvent soe;
		autoByteCpy(&soe.output, str);
		e.data = &soe;
		bool ret = runCscode(ActEvent.ONSERVERCMDOUTPUT, ActMode::BEFORE, e);
		if (ret) {
			VA result = original(handle, str, size);
			e.result = ret;
			e.mode = ActMode::AFTER;
			runCscode(ActEvent.ONSERVERCMDOUTPUT, ActMode::AFTER, e);
			soe.releaseAll();
			return result;
		}
		soe.releaseAll();
		return handle;
	}
	return original(handle, str, size);
}
static VA ONSERVERCMDOUTPUT_SYMS[] = {1, MSSYM_MD5_b5f2f0a753fc527db19ac8199ae8f740, (VA)_CS_ONSERVERCMDOUTPUT};


// 玩家选择表单
static void _CS_ONFORMSELECT(VA _this, VA id, VA handle, ModalFormResponsePacket** fp) {
	auto original = (void(*)(VA, VA, VA, ModalFormResponsePacket**)) * getOriginalData(_CS_ONFORMSELECT);
	ModalFormResponsePacket* fmp = *fp;
	Player* p = SYMCALL(Player*, MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		handle, id, *(char*)((VA)fmp + 16));
	if (p != NULL) {
		UINT fid = fmp->getFormId();
		if (releaseForm(fid)) {
			Events e;
			e.type = EventType::onFormSelect;
			e.mode = ActMode::BEFORE;
			e.result = 0;
			FormSelectEvent fse;
			addPlayerInfo(&fse, p);
			fse.pplayer = p;
			autoByteCpy(&fse.uuid, p->getUuid()->toString().c_str());
			autoByteCpy(&fse.selected, fmp->getSelectStr().c_str());			// 特别鸣谢：sysca11
			fse.formid = fid;
			e.data = &fse;
			bool ret = runCscode(ActEvent.ONFORMSELECT, ActMode::BEFORE, e);
			if (ret) {
				original(_this, id, handle, fp);
				e.result = ret;
				e.mode = ActMode::AFTER;
				runCscode(ActEvent.ONFORMSELECT, ActMode::AFTER, e);
			}
			fse.releaseAll();
			return;
		}
	}
	original(_this, id, handle, fp);
}
static VA ONFORMSELECT_SYMS[] = {1, MSSYM_MD5_8b7f7560f9f8353e6e9b16449ca999d2, (VA)_CS_ONFORMSELECT};

// 玩家操作物品
static bool _CS_ONUSEITEM(void* _this, ItemStack* item, BlockPos* pBlkpos, unsigned __int8 a4, void* v5, Block* pBlk) {
	auto pPlayer = *reinterpret_cast<Player**>(reinterpret_cast<VA>(_this) + 8);
	Events e;
	e.type = EventType::onUseItem;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	UseItemEvent ue;
	addPlayerInfo(&ue, pPlayer);
	ue.pplayer = pPlayer;
	memcpy(&ue.position, pBlkpos->getPosition(), sizeof(BPos3));
	autoByteCpy(&ue.itemname, item->getName().c_str());
	ue.itemid = item->getId();
	ue.itemaux = item->getAuxValue();
	if (pBlk) {
		autoByteCpy(&ue.blockname, pBlk->getLegacyBlock()->getFullName().c_str());
		ue.blockid = pBlk->getLegacyBlock()->getBlockItemID();
	}
	e.data = &ue;
	bool ret = runCscode(ActEvent.ONUSEITEM, ActMode::BEFORE, e);
	if (ret) {
		auto original = (bool(*)(void*, ItemStack*, BlockPos*, unsigned __int8, void*, Block*)) * getOriginalData(_CS_ONUSEITEM);
		ret = original(_this, item, pBlkpos, a4, v5, pBlk);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONUSEITEM, ActMode::AFTER, e);
	}
	ue.releaseAll();
	return ret;
}
static VA ONUSEITEM_SYMS[] = {1, MSSYM_B1QA9useItemOnB1AA8GameModeB2AAA4UEAAB1UE14NAEAVItemStackB2AAE12AEBVBlockPosB2AAA9EAEBVVec3B2AAA9PEBVBlockB3AAAA1Z,
	(VA)_CS_ONUSEITEM};

// 玩家放置方块
static bool _CS_ONPLACEDBLOCK(BlockSource* _this, Block* pBlk, BlockPos* pBlkpos, unsigned __int8 a4, struct Actor* pPlayer, bool _bool) {
	auto original = (bool(*)(BlockSource*, Block*, BlockPos*, unsigned __int8, Actor*, bool)) * getOriginalData(_CS_ONPLACEDBLOCK);
	if (pPlayer && checkIsPlayer(pPlayer)) {
		Player* pp = (Player*)pPlayer;
		Events e;
		e.type = EventType::onPlacedBlock;
		e.mode = ActMode::BEFORE;
		e.result = 0;
		PlacedBlockEvent pe;
		addPlayerInfo(&pe, pp);
		pe.pplayer = pp;
		pe.blockid = pBlk->getLegacyBlock()->getBlockItemID();
		autoByteCpy(&pe.blockname, pBlk->getLegacyBlock()->getFullName().c_str());
		memcpy(&pe.position, pBlkpos->getPosition(), sizeof(BPos3));
		e.data = &pe;
		bool ret = runCscode(ActEvent.ONPLACEDBLOCK, ActMode::BEFORE, e);
		if (ret) {
			ret = original(_this, pBlk, pBlkpos, a4, pPlayer, _bool);
			e.result = ret;
			e.mode = ActMode::AFTER;
			runCscode(ActEvent.ONPLACEDBLOCK, ActMode::AFTER, e);
		}
		pe.releaseAll();
		return ret;
	}
	return original(_this, pBlk, pBlkpos, a4, pPlayer, _bool);
}
static VA ONPLACEDBLOCK_SYMS[] = { 1, MSSYM_B1QA8mayPlaceB1AE11BlockSourceB2AAA4QEAAB1UE10NAEBVBlockB2AAE12AEBVBlockPosB2AAE10EPEAVActorB3AAUA1NB1AA1Z ,
(VA)_CS_ONPLACEDBLOCK};

// 玩家破坏方块
static bool _CS_ONDESTROYBLOCK(void* _this, BlockPos* pBlkpos) {
	auto pPlayer = *reinterpret_cast<Player**>(reinterpret_cast<VA>(_this) + 8);
	auto pBlockSource = *(BlockSource**)(*((VA*)_this + 1) + 800);
	auto pBlk = pBlockSource->getBlock(pBlkpos);
	Events e;
	e.type = EventType::onDestroyBlock;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	DestroyBlockEvent de;
	addPlayerInfo(&de, pPlayer);
	de.pplayer = pPlayer;
	de.blockid = pBlk->getLegacyBlock()->getBlockItemID();
	autoByteCpy(&de.blockname, pBlk->getLegacyBlock()->getFullName().c_str());
	memcpy(&de.position, pBlkpos->getPosition(), sizeof(BPos3));
	e.data = &de;
	bool ret = runCscode(ActEvent.ONDESTROYBLOCK, ActMode::BEFORE, e);
	if (ret) {
		auto original = (bool(*)(void*, BlockPos*)) * getOriginalData(_CS_ONDESTROYBLOCK);
		ret = original(_this, pBlkpos);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONDESTROYBLOCK, ActMode::AFTER, e);
	}
	de.releaseAll();
	return ret;
}
static VA ONDESTROYBLOCK_SYMS[] = {1, MSSYM_B2QUE20destroyBlockInternalB1AA8GameModeB2AAA4AEAAB1UE13NAEBVBlockPosB2AAA1EB1AA1Z,
	(VA)_CS_ONDESTROYBLOCK};

// 玩家开箱准备
static bool _CS_ONCHESTBLOCKUSE(void* _this, Player* pPlayer, BlockPos* pBlkpos) {
	auto pBlockSource = (BlockSource*)((Level*)pPlayer->getLevel())->getDimension(pPlayer->getDimensionId())->getBlockSource();
	auto pBlk = pBlockSource->getBlock(pBlkpos);
	Events e;
	e.type = EventType::onStartOpenChest;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	StartOpenChestEvent de;
	addPlayerInfo(&de, pPlayer);
	de.pplayer = pPlayer;
	de.blockid = pBlk->getLegacyBlock()->getBlockItemID();
	autoByteCpy(&de.blockname, pBlk->getLegacyBlock()->getFullName().c_str());
	memcpy(&de.position, pBlkpos->getPosition(), sizeof(BPos3));
	e.data = &de;
	bool ret = runCscode(ActEvent.ONSTARTOPENCHEST, ActMode::BEFORE, e);
	if (ret) {
		auto original = (bool(*)(void*, Player*, BlockPos*)) * getOriginalData(_CS_ONCHESTBLOCKUSE);
		ret = original(_this, pPlayer, pBlkpos);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONSTARTOPENCHEST, ActMode::AFTER, e);
	}
	de.releaseAll();
	return ret;
}
static VA ONSTARTOPENCHEST_SYMS[] = { 1, MSSYM_B1QA3useB1AE10ChestBlockB2AAA4UEBAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB3AAAA1Z,
	(VA)_CS_ONCHESTBLOCKUSE };

// 玩家开桶准备
static bool _CS_ONBARRELBLOCKUSE(void* _this, Player* pPlayer, BlockPos* pBlkpos) {
	auto pBlockSource = (BlockSource*)((Level*)pPlayer->getLevel())->getDimension(pPlayer->getDimensionId())->getBlockSource();
	auto pBlk = pBlockSource->getBlock(pBlkpos);
	Events e;
	e.type = EventType::onStartOpenBarrel;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	StartOpenBarrelEvent de;
	addPlayerInfo(&de, pPlayer);
	de.pplayer = pPlayer;
	de.blockid = pBlk->getLegacyBlock()->getBlockItemID();
	autoByteCpy(&de.blockname, pBlk->getLegacyBlock()->getFullName().c_str());
	memcpy(&de.position, pBlkpos->getPosition(), sizeof(BPos3));
	e.data = &de;
	bool ret = runCscode(ActEvent.ONSTARTOPENBARREL, ActMode::BEFORE, e);
	if (ret) {
		auto original = (bool(*)(void*, Player*, BlockPos*)) * getOriginalData(_CS_ONBARRELBLOCKUSE);
		ret = original(_this, pPlayer, pBlkpos);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONSTARTOPENBARREL, ActMode::AFTER, e);
	}
	de.releaseAll();
	return ret;
}
static VA ONSTARTOPENBARREL_SYMS[] = {1, MSSYM_B1QA3useB1AE11BarrelBlockB2AAA4UEBAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB3AAAA1Z,
	(VA)_CS_ONBARRELBLOCKUSE};

// 玩家关闭箱子
static void _CS_ONSTOPOPENCHEST(void* _this, Player* pPlayer) {
	auto real_this = reinterpret_cast<void*>(reinterpret_cast<VA>(_this) - 248);
	auto pBlkpos = reinterpret_cast<BlockActor*>(real_this)->getPosition();
	auto pBlockSource = (BlockSource*)((Level*)pPlayer->getLevel())->getDimension(pPlayer->getDimensionId())->getBlockSource();
	auto pBlk = pBlockSource->getBlock(pBlkpos);
	Events e;
	e.type = EventType::onStopOpenChest;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	StopOpenChestEvent de;
	addPlayerInfo(&de, pPlayer);
	de.pplayer = pPlayer;
	de.blockid = pBlk->getLegacyBlock()->getBlockItemID();
	autoByteCpy(&de.blockname, pBlk->getLegacyBlock()->getFullName().c_str());
	memcpy(&de.position, pBlkpos->getPosition(), sizeof(BPos3));
	e.data = &de;
	runCscode(ActEvent.ONSTOPOPENCHEST, ActMode::BEFORE, e);
	auto original = (void(*)(void*, Player*)) * getOriginalData(_CS_ONSTOPOPENCHEST);
	original(_this, pPlayer);
	e.result = true;
	e.mode = ActMode::AFTER;
	runCscode(ActEvent.ONSTOPOPENCHEST, ActMode::AFTER, e);
	de.releaseAll();
}
static VA ONSTOPOPENCHEST_SYMS[] = {1, MSSYM_B1QA8stopOpenB1AE15ChestBlockActorB2AAE15UEAAXAEAVPlayerB3AAAA1Z,
	(VA)_CS_ONSTOPOPENCHEST};

// 玩家关闭木桶
static void _CS_STOPOPENBARREL(void* _this, Player* pPlayer) {
	auto real_this = reinterpret_cast<void*>(reinterpret_cast<VA>(_this) - 248);
	auto pBlkpos = reinterpret_cast<BlockActor*>(real_this)->getPosition();
	auto pBlockSource = (BlockSource*)((Level*)pPlayer->getLevel())->getDimension(pPlayer->getDimensionId())->getBlockSource();
	auto pBlk = pBlockSource->getBlock(pBlkpos);
	Events e;
	e.type = EventType::onStopOpenBarrel;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	StopOpenBarrelEvent de;
	addPlayerInfo(&de, pPlayer);
	de.pplayer = pPlayer;
	de.blockid = pBlk->getLegacyBlock()->getBlockItemID();
	autoByteCpy(&de.blockname, pBlk->getLegacyBlock()->getFullName().c_str());
	memcpy(&de.position, pBlkpos->getPosition(), sizeof(BPos3));
	e.data = &de;
	runCscode(ActEvent.ONSTOPOPENBARREL, ActMode::BEFORE, e);
	auto original = (void(*)(void*, Player*)) * getOriginalData(_CS_STOPOPENBARREL);
	original(_this, pPlayer);
	e.result = true;
	e.mode = ActMode::AFTER;
	runCscode(ActEvent.ONSTOPOPENBARREL, ActMode::AFTER, e);
	de.releaseAll();
}
static VA ONSTOPOPENBARREL_SYMS[] = {1, MSSYM_B1QA8stopOpenB1AE16BarrelBlockActorB2AAE15UEAAXAEAVPlayerB3AAAA1Z,
	(VA)_CS_STOPOPENBARREL};

// 玩家放入取出数量
static void _CS_ONSETSLOT(LevelContainerModel* a1, VA a2) {
	auto original = (void(*)(LevelContainerModel*, VA)) * getOriginalData(_CS_ONSETSLOT);
	VA v3 = *((VA*)a1 + 26);				// IDA LevelContainerModel::_getContainer
	BlockSource* bs = *(BlockSource**)(*(VA*)(v3 + 808) + 72);
	BlockPos* pBlkpos = (BlockPos*)((char*)a1 + 216);
	Block* pBlk = bs->getBlock(pBlkpos);
	short id = pBlk->getLegacyBlock()->getBlockItemID();
	if (id == 54 || id == 130 || id == 146 || id == -203 || id == 205 || id == 218) {	// 非箱子、桶、潜影盒的情况不作处理
		int slot = (int)a2;
		auto v5 = (*(VA(**)(LevelContainerModel*))(*(VA*)a1 + 160))(a1);
		if (v5) {
			ItemStack* pItemStack = (ItemStack*)(*(VA(**)(VA, VA))(*(VA*)v5 + 40))(v5, a2);
			auto nid = pItemStack->getId();
			auto naux = pItemStack->getAuxValue();
			auto nsize = pItemStack->getStackSize();
			auto nname = std::string(pItemStack->getName());
			auto pPlayer = a1->getPlayer();
			Events e;
			e.type = EventType::onSetSlot;
			e.mode = ActMode::BEFORE;
			e.result = 0;
			SetSlotEvent de;
			addPlayerInfo(&de, pPlayer);
			de.pplayer = pPlayer;
			de.itemid = nid;
			de.itemcount = nsize;
			autoByteCpy(&de.itemname, nname.c_str());
			de.itemaux = naux;
			memcpy(&de.position, pBlkpos, sizeof(BPos3));
			de.blockid = id;
			autoByteCpy(&de.blockname, pBlk->getLegacyBlock()->getFullName().c_str());
			de.slot = slot;
			e.data = &de;
			bool ret = runCscode(ActEvent.ONSETSLOT, ActMode::BEFORE, e);
			if (ret) {
				original(a1, a2);
				e.result = true;
				e.mode = ActMode::AFTER;
				runCscode(ActEvent.ONSETSLOT, ActMode::AFTER, e);
			}
			de.releaseAll();
		}
		else
			original(a1, a2);
	}
	else
		original(a1, a2);
}
static VA ONSETSLOT_SYMS[] = {1, MSSYM_B1QE23containerContentChangedB1AE19LevelContainerModelB2AAA6UEAAXHB1AA1Z,
	(VA)_CS_ONSETSLOT};

// 玩家切换维度
static bool _CS_ONCHANGEDIMENSION(void* _this, Player* pPlayer, void* req) {
	Events e;
	e.type = EventType::onChangeDimension;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	ChangeDimensionEvent de;
	addPlayerInfo(&de, pPlayer);
	de.pplayer = pPlayer;
	e.data = &de;
	bool ret = runCscode(ActEvent.ONCHANGEDIMENSION, ActMode::BEFORE, e);
	if (ret) {
		auto original = (bool(*)(void*, Player*, void*)) * getOriginalData(_CS_ONCHANGEDIMENSION);
		ret = original(_this, pPlayer, req);
		e.result = ret;
		if (ret) {
			// 此处刷新玩家信息
			addPlayerInfo(&de, pPlayer);
		}
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONCHANGEDIMENSION, ActMode::AFTER, e);
	}
	de.releaseAll();
	return ret;
}
static VA ONCHANGEDIMENSION_SYMS[] = {1, MSSYM_B2QUE21playerChangeDimensionB1AA5LevelB2AAA4AEAAB1UE11NPEAVPlayerB2AAE26AEAVChangeDimensionRequestB3AAAA1Z ,
	(VA)_CS_ONCHANGEDIMENSION};

// 生物死亡
static void _CS_ONMOBDIE(Mob* _this, void* dmsg) {
	Events e;
	e.type = EventType::onMobDie;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	MobDieEvent de;
	getDamageInfo(_this, dmsg, &de);
	de.pmob = _this;
	e.data = &de;
	bool ret = runCscode(ActEvent.ONMOBDIE, ActMode::BEFORE, e);
	if (ret) {
		auto original = (void(*)(Mob*, void*))*getOriginalData(_CS_ONMOBDIE);
		original(_this, dmsg);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONMOBDIE, ActMode::AFTER, e);
	}
	de.releaseAll();
}
static VA ONMOBDIE_SYMS[] = { 1, MSSYM_B1QA3dieB1AA3MobB2AAE26UEAAXAEBVActorDamageSourceB3AAAA1Z, (VA)_CS_ONMOBDIE };

// 玩家重生
static void _CS_PLAYERRESPAWN(Player* pPlayer) {
	Events e;
	e.type = EventType::onRespawn;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	RespawnEvent de;
	addPlayerInfo(&de, pPlayer);
	de.pplayer = pPlayer;
	e.data = &de;
	bool ret = runCscode(ActEvent.ONRESPAWN, ActMode::BEFORE, e);
	if (ret) {
		auto original = (void(*)(Player*)) * getOriginalData(_CS_PLAYERRESPAWN);
		original(pPlayer);
		e.result = ret;
		e.mode = ActMode::AFTER;
		addPlayerInfo(&de, pPlayer);
		runCscode(ActEvent.ONRESPAWN, ActMode::AFTER, e);
	}
	de.releaseAll();
}
static VA ONRESPAWN_SYMS[] = { 1, MSSYM_B1QA7respawnB1AA6PlayerB2AAA7UEAAXXZ, (VA)_CS_PLAYERRESPAWN };

// 聊天消息
static void _CS_ONCHAT(void* _this, std::string& player_name, std::string& target, std::string& msg, std::string& chat_style) {
	Events e;
	e.type = EventType::onChat;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	ChatEvent de;
	autoByteCpy(&de.playername, player_name.c_str());
	autoByteCpy(&de.target, target.c_str());
	autoByteCpy(&de.msg, msg.c_str());
	autoByteCpy(&de.chatstyle, chat_style.c_str());
	e.data = &de;
	bool ret = runCscode(ActEvent.ONCHAT, ActMode::BEFORE, e);
	if (ret) {
		auto original = (void(*)(void*, std::string&, std::string&, std::string&, std::string&)) * getOriginalData(_CS_ONCHAT);
		original(_this, player_name, target, msg, chat_style);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONCHAT, ActMode::AFTER, e);
	}
	de.releaseAll();
}
static VA ONCHAT_SYMS[] = {1, MSSYM_MD5_ad251f2fd8c27eb22c0c01209e8df83c, (VA)_CS_ONCHAT };

// 输入文本
static void _CS_ONINPUTTEXT(VA _this, VA id, TextPacket* tp) {
	Player* p = SYMCALL(Player*, MSSYM_B2QUE15getServerPlayerB1AE20ServerNetworkHandlerB2AAE20AEAAPEAVServerPlayerB2AAE21AEBVNetworkIdentifierB2AAA1EB1AA1Z,
		_this, id, *((char*)tp + 16));
	Events e;
	e.type = EventType::onInputText;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	InputTextEvent de;
	if (p != NULL) {
		addPlayerInfo(&de, p);
		de.pplayer = p;
	}
	autoByteCpy(&de.msg, tp->toString().c_str());
	e.data = &de;
	bool ret = runCscode(ActEvent.ONINPUTTEXT, ActMode::BEFORE, e);
	if (ret) {
		auto original = (void(*)(VA, VA, TextPacket*)) * getOriginalData(_CS_ONINPUTTEXT);
		original(_this, id, tp);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONINPUTTEXT, ActMode::AFTER, e);
	}
	de.releaseAll();
}
static VA ONINPUTTEXT_SYMS[] = { 1, MSSYM_B1QA6handleB1AE20ServerNetworkHandlerB2AAE26UEAAXAEBVNetworkIdentifierB2AAE14AEBVTextPacketB3AAAA1Z,
	(VA)_CS_ONINPUTTEXT };

// MakeWP
static Player* MakeWP(CommandOrigin& ori) {
	if (ori.getOriginType() == OriginType::Player) {
		return (Player*)ori.getEntity();
	}
	return 0;
}

// 玩家执行指令
static VA _CS_ONINPUTCOMMAND(VA _this, VA mret, std::shared_ptr<CommandContext> x, char a4) {
	auto original = (VA(*)(VA, VA, std::shared_ptr<CommandContext>, char)) * getOriginalData(_CS_ONINPUTCOMMAND);
	Player* p = MakeWP(x->getOrigin());
	if (p) {
		Events e;
		e.type = EventType::onInputCommand;
		e.mode = ActMode::BEFORE;
		e.result = 0;
		InputCommandEvent de;
		if (p != NULL) {
			addPlayerInfo(&de, p);
			de.pplayer = p;
		}
		autoByteCpy(&de.cmd, x->getCmd().c_str());
		VA mcmd = 0;
		e.data = &de;
		bool ret = runCscode(ActEvent.ONINPUTCOMMAND, ActMode::BEFORE, e);
		if (ret) {
			mcmd = original(_this, mret, x, a4);
			e.result = ret;
			e.mode = ActMode::AFTER;
			runCscode(ActEvent.ONINPUTCOMMAND, ActMode::AFTER, e);
		}
		de.releaseAll();
		return mcmd;
	}
	return original(_this, mret, x, a4);
}
static VA ONINPUTCOMMAND_SYMS[] = { 1, MSSYM_B1QE14executeCommandB1AE17MinecraftCommandsB2AAA4QEBAB1QE10AUMCRESULTB2AAA1VB2QDA6sharedB1UA3ptrB1AE15VCommandContextB3AAAA3stdB3AAUA1NB1AA1Z,
	(VA)_CS_ONINPUTCOMMAND };

// 玩家加载名字
THook2(_CS_ONCREATEPLAYER, VA,
	MSSYM_B1QE14onPlayerJoinedB1AE16ServerScoreboardB2AAE15UEAAXAEBVPlayerB3AAAA1Z,
	VA a1, Player* pPlayer) {
	VA hret = original(a1, pPlayer);
	Events e;
	e.type = EventType::onLoadName;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	LoadNameEvent le;
	autoByteCpy(&le.playername, pPlayer->getNameTag().c_str());
	auto uuid = pPlayer->getUuid()->toString();
	autoByteCpy(&le.uuid, uuid.c_str());
	autoByteCpy(&le.xuid, pPlayer->getXuid(p_level).c_str());
#if (COMMERCIAL)
	autoByteCpy(&le.ability, getAbilities(pPlayer).toStyledString().c_str());
#endif
	le.pplayer = pPlayer;
	e.data = &le;
	onlinePlayers[uuid] = pPlayer;
	playerSign[pPlayer] = true;
	bool ret = runCscode(ActEvent.ONLOADNAME, ActMode::BEFORE, e);
	if (ret) {
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONLOADNAME, ActMode::AFTER, e);
	}
	le.releaseAll();
	return hret;
}

// 玩家离开游戏
THook2(_CS_ONPLAYERLEFT, void,
	MSSYM_B2QUE12onPlayerLeftB1AE20ServerNetworkHandlerB2AAE21AEAAXPEAVServerPlayerB3AAUA1NB1AA1Z,
	VA _this, Player* pPlayer, char v3) {
	Events e;
	e.type = EventType::onPlayerLeft;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	PlayerLeftEvent le;
	autoByteCpy(&le.playername, pPlayer->getNameTag().c_str());
	auto uuid = pPlayer->getUuid()->toString();
	autoByteCpy(&le.uuid, uuid.c_str());
	autoByteCpy(&le.xuid, pPlayer->getXuid(p_level).c_str());
#if (COMMERCIAL)
	autoByteCpy(&le.ability, getAbilities(pPlayer).toStyledString().c_str());
#endif
	le.pplayer = pPlayer;
	e.data = &le;
	bool ret = runCscode(ActEvent.ONPLAYERLEFT, ActMode::BEFORE, e);
	playerSign[pPlayer] = false;
	playerSign.erase(pPlayer);
	onlinePlayers[uuid] = NULL;
	onlinePlayers.erase(uuid);
	if (ret) {
		original(_this, pPlayer, v3);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONPLAYERLEFT, ActMode::AFTER, e);
	}
	le.releaseAll();
}



// 此处为防止意外崩溃出现，设置常锁
THook2(_CS_ONLOGOUT, VA,
	MSSYM_B3QQUE13EServerPlayerB2AAA9UEAAPEAXIB1AA1Z,
	Player* a1, VA a2) {
	mleftlock.lock();
	if (playerSign[a1]) {				// 非正常登出游戏用户，执行注销
		playerSign[a1] = false;
		playerSign.erase(a1);
		const std::string* uuid = NULL;
		for (auto& p : onlinePlayers) {
			if (p.second == a1) {
				uuid = &p.first;
				break;
			}
		}
		if (uuid)
			onlinePlayers.erase(*uuid);
	}
	mleftlock.unlock();
	return original(a1, a2);
}

// 玩家移动信息构筑
static VA _CS_ONMOVE(void* _this, Player* pPlayer, char v3, int v4, int v5) {
	auto original = (VA(*)(void*, Player*, char, int, int)) * getOriginalData(_CS_ONMOVE);
	VA reg = (beforecallbacks[ActEvent.ONMOVE] != NULL ? beforecallbacks[ActEvent.ONMOVE]->size() : 0) +
		(aftercallbacks[ActEvent.ONMOVE] != NULL ? aftercallbacks[ActEvent.ONMOVE]->size() : 0);
	if (!reg)
		return original(_this, pPlayer, v3, v4, v5);
	VA reto = 0;
	Events e;
	MoveEvent de;
	e.type = EventType::onMove;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	addPlayerInfo(&de, pPlayer);
	de.pplayer = pPlayer;
	e.data = &de;
	bool ret = runCscode(ActEvent.ONMOVE, ActMode::BEFORE, e);
	if (ret) {
		reto = original(_this, pPlayer, v3, v4, v5);
		e.result = ret;
		runCscode(ActEvent.ONMOVE, ActMode::AFTER, e);
	}
	de.releaseAll();
	return reto;
}
static VA ONMOVE_SYMS[] = {1, MSSYM_B2QQE170MovePlayerPacketB2AAA4QEAAB1AE10AEAVPlayerB2AAE14W4PositionModeB1AA11B1AA2HHB1AA1Z,
	(VA)_CS_ONMOVE};

// 玩家攻击监听
static bool _CS_ONATTACK(Player* pPlayer, Actor* pa) {
	Events e;
	e.type = EventType::onAttack;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	AttackEvent de;
	addPlayerInfo(&de, pPlayer);
	de.pattacker = pPlayer;
	de.pattackedentity = pa;
	memcpy(&de.actorpos, pa->getPos(), sizeof(Vec3));
	autoByteCpy(&de.actorname, pa->getNameTag().c_str());
	autoByteCpy(&de.actortype, pa->getTypeName().c_str());
	e.data = &de;
	bool ret = runCscode(ActEvent.ONATTACK, ActMode::BEFORE, e);
	if (ret) {
		auto original = (bool(*)(Player*, Actor*)) * getOriginalData(_CS_ONATTACK);
		ret = original(pPlayer, pa);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONATTACK, ActMode::AFTER, e);
	}
	de.releaseAll();
	return ret;
}
static VA ONATTACK_SYMS[] = {1, MSSYM_B1QA6attackB1AA6PlayerB2AAA4UEAAB1UE10NAEAVActorB3AAAA1Z,
	(VA)_CS_ONATTACK};

// 全图范围爆炸监听
static void _CS_ONLEVELEXPLODE(VA _this, BlockSource* a2, Actor* a3, Vec3* a4, float a5, bool a6, bool a7, float a8, bool a9) {
	Events e;
	e.type = EventType::onLevelExplode;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	LevelExplodeEvent de;
	memcpy(&de.position, a4, sizeof(Vec3));
	int did = a2 ? a2->getDimensionId() : -1;
	de.dimensionid = did;
	autoByteCpy(&de.dimension, toDimenStr(did).c_str());
	if (a3) {
		autoByteCpy(&de.entity, a3->getEntityTypeName().c_str());
		de.entityid = a3->getEntityTypeId();
		int i = a3->getDimensionId();
		de.dimensionid = i;
		autoByteCpy(&de.dimension, toDimenStr(i).c_str());
	}
	de.explodepower = a5;
	e.data = &de;
	bool ret = runCscode(ActEvent.ONLEVELEXPLODE, ActMode::BEFORE, e);
	if (ret) {
		auto original = (void(*)(VA, BlockSource*, Actor*, Vec3*, float, bool, bool, float, bool)) * getOriginalData(_CS_ONLEVELEXPLODE);
		original(_this, a2, a3, a4, a5, a6, a7, a8, a9);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONLEVELEXPLODE, ActMode::AFTER, e);
	}
	de.releaseAll();
}
// 重生锚爆炸监听
static bool _CS_SETRESPWNEXPLOREDE(Player* pPlayer, BlockPos* a2, BlockSource* a3, Level* a4) {
	auto original = (bool(*)(Player*, BlockPos*, BlockSource*, Level*)) * getOriginalData(_CS_SETRESPWNEXPLOREDE);
	auto v8 = a3->getBlock(a2);
	auto v9 = (VA*)*((VA*)v8 + 2);
	VA qwt = SYM_OBJECT(VA, (MSSYM_B1QE19RespawnAnchorChargeB1AE13VanillaStatesB2AAA23VB2QDE16ItemStateVariantB1AA1HB2AAA1B + 8));
	if (!*(char*)(*v9 + 32 * qwt + 460)
		|| !(((unsigned int)*((unsigned __int16*)v8 + 4) >> (*(char*)(*v9 + 32 * qwt + 444)
			- *(char*)(*v9 + 32 * qwt + 448)
			+ 1)) & (0xFFFF >> (*(char*)(*v9 + 32 * qwt + 440)
				- *(char*)(*v9 + 32 * qwt + 448)))))
	{
		return original(pPlayer, a2, a3, a4);
	}
	struct VA_tmp { VA v; };
	if (a3->getDimensionId() != 1) {
		if (!*(char*)(*((VA*)pPlayer + 102) + 7664)) {
			float pw = SYM_OBJECT(float, MSSYM_B2UUA4realB1AA840a00000);
			if (!*(char*)&(((VA_tmp*)a4)[958].v)) {
				if (pw != 0.0) {
					std::string blkname = v8->getLegacyBlock()->getFullName();
					Events e;
					e.type = EventType::onLevelExplode;
					e.mode = ActMode::BEFORE;
					e.result = 0;
					LevelExplodeEvent de;
					de.blockid = v8->getLegacyBlock()->getBlockItemID();
					autoByteCpy(&de.blockname, v8->getLegacyBlock()->getFullName().c_str());
					de.dimensionid = a3->getDimensionId();
					autoByteCpy(&de.dimension, toDimenStr(de.dimensionid).c_str());
					de.position.x = (float)a2->getPosition()->x;
					de.position.y = (float)a2->getPosition()->y;
					de.position.z = (float)a2->getPosition()->z;
					de.explodepower = pw;
					e.data = &de;
					bool ret = runCscode(ActEvent.ONLEVELEXPLODE, ActMode::BEFORE, e);
					if (ret) {
						ret = original(pPlayer, a2, a3, a4);
						e.result = ret;
						e.mode = ActMode::AFTER;
						runCscode(ActEvent.ONLEVELEXPLODE, ActMode::AFTER, e);
					}
					de.releaseAll();
					return ret;
				}
			}
		}
	}
	return original(pPlayer, a2, a3, a4);
}
static VA ONLEVELEXPLODE_SYMS[] = { 2, MSSYM_B1QA7explodeB1AA5LevelB2AAE20QEAAXAEAVBlockSourceB2AAA9PEAVActorB2AAA8AEBVVec3B2AAA1MB1UA4N3M3B1AA1Z,
	(VA)_CS_ONLEVELEXPLODE,
	MSSYM_B1QE11trySetSpawnB1AE18RespawnAnchorBlockB2AAA2CAB1UE11NAEAVPlayerB2AAE12AEBVBlockPosB2AAE15AEAVBlockSourceB2AAA9AEAVLevelB3AAAA1Z,
	(VA)_CS_SETRESPWNEXPLOREDE };

// 玩家切换护甲（不含主副手）
static VA _CS_ONSETARMOR(Player* p, int slot, ItemStack *i) {
	auto original = (VA(*)(Player *, int, ItemStack *)) * getOriginalData(_CS_ONSETARMOR);
	if (checkIsPlayer(p)) {
		ItemStack* pItemStack = i;
		auto nid = pItemStack->getId();
		auto naux = pItemStack->getAuxValue();
		auto nsize = pItemStack->getStackSize();
		auto nname = std::string(pItemStack->getName());
		auto pPlayer = p;
		Events e;
		e.type = EventType::onEquippedArmor;
		e.mode = ActMode::BEFORE;
		e.result = 0;
		EquippedArmorEvent de;
		addPlayerInfo(&de, pPlayer);
		de.pplayer = pPlayer;
		de.itemid = nid;
		de.itemcount = nsize;
		autoByteCpy(&de.itemname, nname.c_str());
		de.itemaux = naux;
		de.slot = slot;
		e.data = &de;
		bool ret = runCscode(ActEvent.ONEQUIPPEDARMOR, ActMode::BEFORE, e);
		if (ret) {
			VA ret = original(p, slot, i);
			e.result = true;
			e.mode = ActMode::AFTER;
			runCscode(ActEvent.ONEQUIPPEDARMOR, ActMode::AFTER, e);
			return ret;
		}
		de.releaseAll();
		return 0;
	}
	return original(p, slot, i);
}
static VA ONSETARMOR_SYMS[] = { 1, MSSYM_B1QA8setArmorB1AE12ServerPlayerB2AAE16UEAAXW4ArmorSlotB2AAE13AEBVItemStackB3AAAA1Z,
	(VA)_CS_ONSETARMOR };

//玩家升级
static void _CS_ONLEVELUP(Player* pl, int a1) {
	auto original = (void(*)(Player*, int)) * getOriginalData(_CS_ONLEVELUP);
	Events e;
	e.type = EventType::onLevelUp;
	e.mode = ActMode::BEFORE;
	e.result = 0;
	LevelUpEvent le;
	addPlayerInfo(&le, pl);
	le.pplayer = pl;
	le.lv = a1;
	e.data = &le;
	bool ret = runCscode(ActEvent.ONLEVELUP, ActMode::BEFORE, e);
	if (ret) {
		original(pl, a1);
		e.result = ret;
		e.mode = ActMode::AFTER;
		runCscode(ActEvent.ONLEVELUP, ActMode::AFTER, e);
	}
}
static VA ONLEVELUP_SYMS[] = { 1, MSSYM_B1QA9addLevelsB1AA6PlayerB2AAA6UEAAXHB1AA1Z,
	(VA)_CS_ONLEVELUP };

// 初始化各类hook的事件绑定，基于构造函数
static struct EventSymsInit{
public:
	EventSymsInit() {
		sListens[ActEvent.ONSERVERCMD] = ONSERVERCMD_SYMS;
		sListens[ActEvent.ONSERVERCMDOUTPUT] = ONSERVERCMDOUTPUT_SYMS;
		sListens[ActEvent.ONFORMSELECT] = ONFORMSELECT_SYMS;
		sListens[ActEvent.ONUSEITEM] = ONUSEITEM_SYMS;
		sListens[ActEvent.ONMOVE] = ONMOVE_SYMS;
		sListens[ActEvent.ONATTACK] = ONATTACK_SYMS;
		sListens[ActEvent.ONPLACEDBLOCK] = ONPLACEDBLOCK_SYMS;
		sListens[ActEvent.ONDESTROYBLOCK] = ONDESTROYBLOCK_SYMS;
		sListens[ActEvent.ONSTARTOPENCHEST] = ONSTARTOPENCHEST_SYMS;
		sListens[ActEvent.ONSTARTOPENBARREL] = ONSTARTOPENBARREL_SYMS;
		sListens[ActEvent.ONCHANGEDIMENSION] = ONCHANGEDIMENSION_SYMS;
		isListened[ActEvent.ONLOADNAME] = true;
		isListened[ActEvent.ONPLAYERLEFT] = true;
		sListens[ActEvent.ONSTOPOPENCHEST] = ONSTOPOPENCHEST_SYMS;
		sListens[ActEvent.ONSTOPOPENBARREL] = ONSTOPOPENBARREL_SYMS;
		sListens[ActEvent.ONSETSLOT] = ONSETSLOT_SYMS;
		sListens[ActEvent.ONMOBDIE] = ONMOBDIE_SYMS;
		sListens[ActEvent.ONRESPAWN] = ONRESPAWN_SYMS;
		sListens[ActEvent.ONCHAT] = ONCHAT_SYMS;
		sListens[ActEvent.ONINPUTTEXT] = ONINPUTTEXT_SYMS;
		sListens[ActEvent.ONINPUTCOMMAND] = ONINPUTCOMMAND_SYMS;
		sListens[ActEvent.ONLEVELEXPLODE] = ONLEVELEXPLODE_SYMS;
		sListens[ActEvent.ONEQUIPPEDARMOR] = ONSETARMOR_SYMS;
		sListens[ActEvent.ONLEVELUP] = ONLEVELUP_SYMS;
#if (COMMERCIAL)
		isListened[ActEvent.ONMOBHURT] = true;
		isListened[ActEvent.ONBLOCKCMD] = true;
		isListened[ActEvent.ONNPCCMD] = true;
		isListened[ActEvent.ONCOMMANDBLOCKUPDATE] = true;
#endif
	}
} _EventSymsInit;

static char localpath[MAX_PATH] = { 0 };

// 获取BDS完整程序路径
static std::string getLocalPath() {
	if (!localpath[0]) {
		GetModuleFileNameA(NULL, localpath, _countof(localpath));
		for (size_t l = strlen(localpath); l != 0; l--) {
			if (localpath[l] == '\\') {
				localpath[l] = localpath[l + 1] = localpath[l + 2] = 0;
				break;
			}
		}
	}
	return std::string(localpath);
}

static bool inited = false;
static VA attcount = 0;

static ICLRMetaHost* pMetaHost = nullptr;
static ICLRMetaHostPolicy* pMetaHostPolicy = nullptr;
static ICLRRuntimeHost* pRuntimeHost = nullptr;
static ICLRRuntimeInfo* pRuntimeInfo = nullptr;

// 初始化.Net环境
static void initNetFramework() {
	SetCurrentDirectoryA(getLocalPath().c_str());
	DWORD dwRet = 0;
	//wchar_t curDir[256] = { 0 };
	HRESULT hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&pMetaHost);
	hr = pMetaHost->GetRuntime(L"v4.0.30319", IID_PPV_ARGS(&pRuntimeInfo));
	if (!FAILED(hr))
	{
		hr = pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&pRuntimeHost));
		if (FAILED(hr))
			return;
		hr = pRuntimeHost->Start();
		if (FAILED(hr))
			return;
		std::wstring curDllandVer = GetDllPathandVersion();	// 获取平台路径和版本，逗号分隔
		// 局部方法，加载所有csr插件
		{
			std::string path = "CSR";	// 固定目录 - CSR
			std::string pair = path + "\\*.csr.dll";
			WIN32_FIND_DATAA ffd;
			HANDLE dfh = FindFirstFileA(pair.c_str(), &ffd);
			if (INVALID_HANDLE_VALUE != dfh) {
				do
				{
					if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					{
						std::string fileName = std::string(path + "\\" + ffd.cFileName);
						int len = MultiByteToWideChar(CP_ACP, 0, fileName.c_str(), (int)fileName.length(), NULL, 0);
						LPWSTR w_str = new WCHAR[(size_t)len + 1];
						w_str[len] = L'\0';
						MultiByteToWideChar(CP_ACP, 0, fileName.c_str(), (int)fileName.length(), w_str, len);
						LPCWSTR dllName = w_str;
						std::wcout << L"[CSR] load " << dllName << std::endl;
						hr = pRuntimeHost->ExecuteInDefaultAppDomain(dllName,	// 插件实际路径
							L"CSR.Plugin",										// 插件通用类名
							L"onServerStart",									// 插件通用初始化接口
							curDllandVer.c_str(),								// 回传lib和版本号
							&dwRet);
						if (FAILED(hr)) {
							wprintf(L"[File] %s api load failed.\n", dllName);
							continue;
						}
					}
				} while (FindNextFileA(dfh, &ffd) != 0);
				FindClose(dfh);
			}
		}
	}
	else {
		releaseNetFramework();
	}
}

// 卸载.Net环境
static void releaseNetFramework() {
	beforecallbacks.clear();
	aftercallbacks.clear();
	if (pRuntimeHost != nullptr)
		pRuntimeHost->Stop();
	if (pRuntimeInfo != nullptr)
	{
		pRuntimeInfo->Release();
		pRuntimeInfo = nullptr;
	}
	if (pRuntimeHost != nullptr)
	{
		pRuntimeHost->Release();
		pRuntimeHost = nullptr;
	}
	if (pMetaHost != nullptr)
	{
		pMetaHost->Release();
		pMetaHost = nullptr;
	}
}

// 附加调用函数
static std::unordered_map<std::string, void*> extraApi;

static void initExtraApi() {
#if (COMMERCIAL)
	extraApi["getStructure"] = &getStructure;
	extraApi["setStructure"] = &setStructure;
	extraApi["getPlayerAbilities"] = &getPlayerAbilities;
	extraApi["setPlayerAbilities"] = &setPlayerAbilities;
	extraApi["getPlayerAttributes"] = &getPlayerAttributes;
	extraApi["setPlayerTempAttributes"] = &setPlayerTempAttributes;
	extraApi["getPlayerMaxAttributes"] = &getPlayerMaxAttributes;
	extraApi["setPlayerMaxAttributes"] = &setPlayerMaxAttributes;
	extraApi["getPlayerItems"] = &getPlayerItems;
	extraApi["setPlayerItems"] = &setPlayerItems;
	extraApi["getPlayerSelectedItem"] = &getPlayerSelectedItem;
	extraApi["addPlayerItemEx"] = &addPlayerItemEx;
	extraApi["addPlayerItem"] = &addPlayerItem;
	extraApi["getPlayerEffects"] = &getPlayerEffects;
	extraApi["setPlayerEffects"] = &setPlayerEffects;
	extraApi["setPlayerBossBar"] = &setPlayerBossBar;
	extraApi["removePlayerBossBar"] = &removePlayerBossBar;
	extraApi["transferserver"] = &transferserver;
	extraApi["teleport"] = &teleport;
	extraApi["setPlayerSidebar"] = &setPlayerSidebar;
	extraApi["removePlayerSidebar"] = &removePlayerSidebar;
	extraApi["getPlayerPermissionAndGametype"] = &getPlayerPermissionAndGametype;
	extraApi["setPlayerPermissionAndGametype"] = &setPlayerPermissionAndGametype;
	extraApi["getExBlock"] = &getExBlock;
	extraApi["setExBlock"] = &setExBlock;
#endif
}

void* getExtraAPI(const char* apiname) {
	std::string k = std::string(apiname);
	return extraApi[k];
}




// 初始化工作
static void initMods()
{
	if (inited) {
		return;
	}
	inited = true;
	initNetFramework();
}

// 底层相关

static std::unordered_map<void*, void**> hooks;

// 获取指定原型存储位置
void** getOriginalData(void* hook) {
	return hooks[hook];
}

// 挂载hook
HookErrorCode mTHook2(RVA sym, void* hook) {
	hooks[hook] = new void* [1]{ 0 };
	void** org = hooks[hook];
	*org = ((char*)GetModuleHandle(NULL)) + sym;
	return Hook<void*>(org, hook);
}

unsigned long long dlsym(int rva) {
	return (VA)GetModuleHandle(NULL) + rva;
}

bool cshook(int rva, void* hook, void** org) {
	*org = ((char*)GetModuleHandle(NULL)) + rva;
	return Hook<void*>(org, hook) == HookErrorCode::ERR_SUCCESS;
}

bool csunhook(void* hook, void** org) {
	return UnHook<void*>(org, hook) == HookErrorCode::ERR_SUCCESS;
}


void init() {
#if (!COMMERCIAL)
	std::cout << u8"{[插件] Net插件运行平台(社区版)已装载。此平台基于LGPL协议发行。" << std::endl;
#else
	std::cout << u8"{[插件] Net插件运行平台已装载。" << std::endl;
#endif
	std::wcout << L"version=" << VERSION << std::endl;
}

void exit() {
	releaseNetFramework();
}
