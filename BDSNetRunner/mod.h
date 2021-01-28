#pragma once
#include "Events.h"

// 社区版/商业版
#define COMMERCIAL 0

// 标识贡献者 05007
#define MODULE_05007 1
// 标识贡献者 kengwang
#define MODULE_KENGWANG 1

extern void init();
extern void exit();
extern void initMods();

#define MCCSAPI extern "C" __declspec (dllexport)
#define MCCPPAPI extern "C" __declspec (dllexport)


// 设置c#一个通用指针
MCCSAPI void setSharePtr(const char*, void*);
// 获取c#一个通用指针
MCCSAPI void* getSharePtr(const char*);
// 移除c#一个通用指针
MCCSAPI void* removeSharePtr(const char*);

// 注册c#事件发生前回调
MCCSAPI bool addBeforeActListener(const char*, bool(*)(Events));
// 注册c#事件发生后回调
MCCSAPI bool addAfterActListener(const char*, bool(*)(Events));
// 移除c#事件发生前回调
MCCSAPI bool removeBeforeActListener(const char*, bool(*)(Events));
// 移除c#事件发生后回调
MCCSAPI bool removeAfterActListener(const char*, bool(*)(Events));
// 设置一个全局指令说明
MCCSAPI void setCommandDescribeEx(const char*, const char*, char, char, char);
// 执行后端指令
MCCSAPI bool runcmd(const char*);
// 发送一个命令输出消息（末尾接换行符）
MCCSAPI void logout(const char*);
// 获取在线玩家列表
MCCSAPI std::string getOnLinePlayers();
// 重设新名字
MCCSAPI bool reNameByUuid(const char*, const char*);
// 获取一个在线玩家信息
MCCSAPI std::string selectPlayer(const char*);
// 模拟喊话
MCCSAPI bool talkAs(const char*, const char*);
// 模拟指令
MCCSAPI bool runcmdAs(const char*, const char*);
// 发送指定玩家一个表单
MCCSAPI UINT sendSimpleForm(char*, char*, char*, char*);
// 发送指定玩家一个模板对话框
MCCSAPI UINT sendModalForm(char*, char*, char*, char*, char*);
// 发送指定玩家一个自定义GUI界面
MCCSAPI UINT sendCustomForm(char*, char*);
// 销毁已使用的id
MCCSAPI bool releaseForm(unsigned);
// 增加玩家一个简单物品
MCCSAPI bool addPlayerItem(const char*, int, short, char);
// 断开一个在线玩家的连接
MCCSAPI bool disconnectClient(const char*, const char*);
// 发送给玩家一个文本
MCCSAPI bool sendText(const char*, const char*);
// 请求执行一段行为包脚本
MCCSAPI void JSErunScript(const char*, void(*)(bool));
// 请求发送一个自定义行为包脚本事件广播
MCCSAPI void JSEfireCustomEvent(const char*, const char*, void(*)(bool));

// 从此处获取额外API
MCCSAPI void* getExtraAPI(const char*);

// 从此处获取类组件的相关方法
MCCSAPI void* mcComponentAPI(const char*);

// 来自社区贡献

#if MODULE_05007
// 获取计分板值
MCCPPAPI int getscoreboard(Player*, std::string);
// C#可用获取计分板值
MCCSAPI int getscoreboardValue(const char*, const char*);
// 设置计分板值
MCCPPAPI bool setscoreboard(Player*, std::string, int);
// C#可用设置计分板值
MCCSAPI bool setscoreboardValue(const char*, const char*, int);
#endif

#if MODULE_KENGWANG
// 设置服务器的显示名信息
MCCSAPI bool setServerMotd(const char*, bool);
#endif

#if (COMMERCIAL)
// 调用原型：获取一个结构
typedef std::string (**getStructureFunc)(int, const char*, const char*, bool, bool);
// 调用原型：设置一个结构到指定位置
typedef bool (**setStructureFunc)(const char*, int, const char*, char, bool, bool);
// 调用原型：获取玩家能力表
typedef std::string (**getPlayerAbilitiesFunc)(const char*);
// 调用原型：设置玩家能力表
typedef bool (**setPlayerAbilitiesFunc)(const char*, const char*);
// 调用原型：获取玩家属性表
typedef std::string (**getPlayerAttributesFunc)(const char*);
// 调用原型：设置玩家属性临时值表
typedef bool (**setPlayerTempAttributesFunc)(const char*, const char*);
// 调用原型：获取玩家属性上限值表
typedef std::string (**getPlayerMaxAttributesFunc)(const char*);
// 调用原型：设置玩家属性上限值表
typedef bool (**setPlayerMaxAttributesFunc)(const char*, const char*);
// 调用原型：获取玩家所有物品列表
typedef std::string (**getPlayerItemsFunc)(const char*);
// 调用原型：设置玩家所有物品列表
typedef bool (**setPlayerItemsFunc)(const char*, const char*);
// 调用原型：获取玩家当前选中项信息
typedef std::string (**getPlayerSelectedItemFunc)(const char*);
// 调用原型：增加玩家一个物品
typedef bool (**addPlayerItemExFunc)(const char*, const char*);
// 调用原型：获取玩家所有效果列表
typedef std::string (**getPlayerEffectsFunc)(const char*);
// 调用原型：设置玩家所有效果列表
typedef bool (**setPlayerEffectsFunc)(const char*, const char*);
// 调用原型：设置玩家自定义血条
typedef bool (**setPlayerBossBarFunc)(const char*, const char*, float);
// 调用原型：清除玩家自定义血条
typedef bool (**removePlayerBossBarFunc)(const char*);
// 调用原型：传送玩家至指定服务器
typedef bool (**transferserverFunc)(const char*, const char*, int);
// 调用原型：传送玩家至指定坐标和维度
typedef bool (**teleportFunc)(const char*, float, float, float, int);
// 调用原型：设置玩家自定义侧边栏临时计分板
typedef bool (**setPlayerSidebarFunc)(const char*, const char*, const char*);
// 调用原型：清除玩家自定义侧边栏
typedef bool (**removePlayerSidebarFunc)(const char*);
// 调用原型：获取玩家权限与游戏模式
typedef std::string (**getPlayerPermissionAndGametypeFunc)(const char*);
// 调用原型：设置玩家权限与游戏模式
typedef bool (**setPlayerPermissionAndGametypeFunc)(const char*, const char*);
#endif

// 返回实际RVA对应地址
MCCSAPI unsigned long long dlsym(int rva);
// 注册c#可用的hook函数
MCCSAPI bool cshook(int rva, void* hook, void** org);
// 注册c#可用的unhook函数
MCCSAPI bool csunhook(void* hook, void** org);
// 读特定段内存硬编码
MCCSAPI bool readHardMemory(int rva, unsigned char* odata, int size);
// 写特定段内存硬编码
MCCSAPI bool writeHardMemory(int rva, unsigned char* ndata, int size);
