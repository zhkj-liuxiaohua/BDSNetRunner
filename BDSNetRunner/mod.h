#pragma once
#include "Events.h"

// ������/��ҵ��
#define COMMERCIAL 0

// ��ʶ������ 05007
#define MODULE_05007 1
// ��ʶ������ kengwang
#define MODULE_KENGWANG 1

extern void init();
extern void exit();
extern void initMods();

#define MCCSAPI extern "C" __declspec (dllexport)
#define MCCPPAPI extern "C" __declspec (dllexport)


// ����c#һ��ͨ��ָ��
MCCSAPI void setSharePtr(const char*, void*);
// ��ȡc#һ��ͨ��ָ��
MCCSAPI void* getSharePtr(const char*);
// �Ƴ�c#һ��ͨ��ָ��
MCCSAPI void* removeSharePtr(const char*);

// ע��c#�¼�����ǰ�ص�
MCCSAPI bool addBeforeActListener(const char*, bool(*)(Events));
// ע��c#�¼�������ص�
MCCSAPI bool addAfterActListener(const char*, bool(*)(Events));
// �Ƴ�c#�¼�����ǰ�ص�
MCCSAPI bool removeBeforeActListener(const char*, bool(*)(Events));
// �Ƴ�c#�¼�������ص�
MCCSAPI bool removeAfterActListener(const char*, bool(*)(Events));
// ����һ��ȫ��ָ��˵��
MCCSAPI void setCommandDescribeEx(const char*, const char*, char, char, char);
// ִ�к��ָ��
MCCSAPI bool runcmd(const char*);
// ����һ�����������Ϣ��ĩβ�ӻ��з���
MCCSAPI void logout(const char*);
// ��ȡ��������б�
MCCSAPI std::string getOnLinePlayers();
// ����������
MCCSAPI bool reNameByUuid(const char*, const char*);
// ��ȡһ�����������Ϣ
MCCSAPI std::string selectPlayer(const char*);
// ģ�⺰��
MCCSAPI bool talkAs(const char*, const char*);
// ģ��ָ��
MCCSAPI bool runcmdAs(const char*, const char*);
// ����ָ�����һ����
MCCSAPI UINT sendSimpleForm(char*, char*, char*, char*);
// ����ָ�����һ��ģ��Ի���
MCCSAPI UINT sendModalForm(char*, char*, char*, char*, char*);
// ����ָ�����һ���Զ���GUI����
MCCSAPI UINT sendCustomForm(char*, char*);
// ������ʹ�õ�id
MCCSAPI bool releaseForm(unsigned);
// �������һ������Ʒ
MCCSAPI bool addPlayerItem(const char*, int, short, char);
// �Ͽ�һ��������ҵ�����
MCCSAPI bool disconnectClient(const char*, const char*);
// ���͸����һ���ı�
MCCSAPI bool sendText(const char*, const char*);
// ����ִ��һ����Ϊ���ű�
MCCSAPI void JSErunScript(const char*, void(*)(bool));
// ������һ���Զ�����Ϊ���ű��¼��㲥
MCCSAPI void JSEfireCustomEvent(const char*, const char*, void(*)(bool));
// ��ȡ���߼Ʒְ�ֵ
MCCSAPI int getscoreById(__int64, const char*);
// �������߼Ʒְ�ֵ
MCCSAPI int setscoreById(__int64, const char*, int);
// ����һ��������tick
MCCSAPI void postTick(void(*)());

// �Ӵ˴���ȡ����API
MCCSAPI void* getExtraAPI(const char*);

// �Ӵ˴���ȡ���������ط���
MCCSAPI void* mcComponentAPI(const char*);

// ������������

#if MODULE_05007
// ��ȡ�Ʒְ�ֵ
MCCPPAPI int getscoreboard(Player*, std::string);
// C#���û�ȡ�Ʒְ�ֵ
MCCSAPI int getscoreboardValue(const char*, const char*);
// ���üƷְ�ֵ
MCCPPAPI bool setscoreboard(Player*, std::string, int);
// C#�������üƷְ�ֵ
MCCSAPI bool setscoreboardValue(const char*, const char*, int);
#endif

#if MODULE_KENGWANG
// ���÷���������ʾ����Ϣ
MCCSAPI bool setServerMotd(const char*, bool);
#endif

#if (COMMERCIAL)
// ����ԭ�ͣ���ȡһ���ṹ
typedef std::string (**getStructureFunc)(int, const char*, const char*, bool, bool);
// ����ԭ�ͣ�����һ���ṹ��ָ��λ��
typedef bool (**setStructureFunc)(const char*, int, const char*, char, bool, bool);
// ����ԭ�ͣ���ȡ���������
typedef std::string (**getPlayerAbilitiesFunc)(const char*);
// ����ԭ�ͣ��������������
typedef bool (**setPlayerAbilitiesFunc)(const char*, const char*);
// ����ԭ�ͣ���ȡ������Ա�
typedef std::string (**getPlayerAttributesFunc)(const char*);
// ����ԭ�ͣ��������������ʱֵ��
typedef bool (**setPlayerTempAttributesFunc)(const char*, const char*);
// ����ԭ�ͣ���ȡ�����������ֵ��
typedef std::string (**getPlayerMaxAttributesFunc)(const char*);
// ����ԭ�ͣ����������������ֵ��
typedef bool (**setPlayerMaxAttributesFunc)(const char*, const char*);
// ����ԭ�ͣ���ȡ���������Ʒ�б�
typedef std::string (**getPlayerItemsFunc)(const char*);
// ����ԭ�ͣ��������������Ʒ�б�
typedef bool (**setPlayerItemsFunc)(const char*, const char*);
// ����ԭ�ͣ���ȡ��ҵ�ǰѡ������Ϣ
typedef std::string (**getPlayerSelectedItemFunc)(const char*);
// ����ԭ�ͣ��������һ����Ʒ
typedef bool (**addPlayerItemExFunc)(const char*, const char*);
// ����ԭ�ͣ���ȡ�������Ч���б�
typedef std::string (**getPlayerEffectsFunc)(const char*);
// ����ԭ�ͣ������������Ч���б�
typedef bool (**setPlayerEffectsFunc)(const char*, const char*);
// ����ԭ�ͣ���������Զ���Ѫ��
typedef bool (**setPlayerBossBarFunc)(const char*, const char*, float);
// ����ԭ�ͣ��������Զ���Ѫ��
typedef bool (**removePlayerBossBarFunc)(const char*);
// ����ԭ�ͣ����������ָ��������
typedef bool (**transferserverFunc)(const char*, const char*, int);
// ����ԭ�ͣ����������ָ�������ά��
typedef bool (**teleportFunc)(const char*, float, float, float, int);
// ����ԭ�ͣ���������Զ���������ʱ�Ʒְ�
typedef bool (**setPlayerSidebarFunc)(const char*, const char*, const char*);
// ����ԭ�ͣ��������Զ�������
typedef bool (**removePlayerSidebarFunc)(const char*);
// ����ԭ�ͣ���ȡ���Ȩ������Ϸģʽ
typedef std::string (**getPlayerPermissionAndGametypeFunc)(const char*);
// ����ԭ�ͣ��������Ȩ������Ϸģʽ
typedef bool (**setPlayerPermissionAndGametypeFunc)(const char*, const char*);
// ����ԭ�ͣ���ȡ���мƷְ�Ʒ���
typedef std::string(**getAllScoreFunc)();
// ����ԭ�ͣ��������мƷְ�Ʒ���
typedef bool (**setAllScoreFunc)(const char*);
// ����ԭ�ͣ���ȡ��ͼ����λ��������ɫ����
typedef std::string(**getMapColorsFunc)(int, int, int, int);
// ����ԭ�ͣ�������ͼ���������������
typedef std::string(**exportPlayersDataFunc)();
// ����ԭ�ͣ����������������ͼ
typedef bool(**importPlayersDataFunc)(const char*);
#endif

// ����ʵ��RVA��Ӧ��ַ
MCCSAPI unsigned long long dlsym(int rva);
// ע��c#���õ�hook����
MCCSAPI bool cshook(int rva, void* hook, void** org);
// ע��c#���õ�unhook����
MCCSAPI bool csunhook(void* hook, void** org);
// ���ض����ڴ�Ӳ����
MCCSAPI bool readHardMemory(int rva, unsigned char* odata, int size);
// д�ض����ڴ�Ӳ����
MCCSAPI bool writeHardMemory(int rva, unsigned char* ndata, int size);
