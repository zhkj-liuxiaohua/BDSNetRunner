#pragma once
#include "BDS.hpp"

enum class EventType : UINT16 {
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
	onLevelExplode = 26,
	onEquippedArmor = 27,
	onLevelUp = 28,
	onPistonPush = 29,
	onChestPair = 30,
	onMobSpawnCheck = 31,
	onDropItem = 32,
	onPickUpItem = 33,
	onScoreChanged = 34,
	onScriptEngineInit = 35,
	onScriptEngineLog = 36,
	onScriptEngineCmd = 37,
	onScoreboardInit = 38
};

// ����ģʽ
enum class ActMode : UINT16
{
	BEFORE = 0,
	AFTER = 1
};

// �����¼��ؼ���
struct ACTEVENT {
	const std::string ONSERVERCMD = u8"onServerCmd";
	const std::string ONSERVERCMDOUTPUT = u8"onServerCmdOutput";
	const std::string ONFORMSELECT = u8"onFormSelect";
	const std::string ONUSEITEM = u8"onUseItem";
	const std::string ONMOVE = u8"onMove";
	const std::string ONATTACK = u8"onAttack";
	const std::string ONPLACEDBLOCK = u8"onPlacedBlock";
	const std::string ONDESTROYBLOCK = u8"onDestroyBlock";
	const std::string ONSTARTOPENCHEST = u8"onStartOpenChest";
	const std::string ONSTARTOPENBARREL = u8"onStartOpenBarrel";
	const std::string ONCHANGEDIMENSION = u8"onChangeDimension";
	const std::string ONLOADNAME = u8"onLoadName";
	const std::string ONPLAYERLEFT = u8"onPlayerLeft";
	const std::string ONSTOPOPENCHEST = u8"onStopOpenChest";
	const std::string ONSTOPOPENBARREL = u8"onStopOpenBarrel";
	const std::string ONSETSLOT = u8"onSetSlot";
	const std::string ONMOBDIE = u8"onMobDie";
	const std::string ONRESPAWN = u8"onRespawn";
	const std::string ONCHAT = u8"onChat";
	const std::string ONINPUTTEXT = u8"onInputText";
	const std::string ONINPUTCOMMAND = u8"onInputCommand";
	const std::string ONLEVELEXPLODE = u8"onLevelExplode";
	const std::string ONEQUIPPEDARMOR = u8"onEquippedArmor";
	const std::string ONLEVELUP = u8"onLevelUp";
	const std::string ONPISTONPUSH = u8"onPistonPush";
	const std::string ONCHESTPAIR = u8"onChestPair";
	const std::string ONMOBSPAWNCHECK = u8"onMobSpawnCheck";
	const std::string ONDROPITEM = u8"onDropItem";
	const std::string ONPICKUPITEM = u8"onPickUpItem";
	const std::string ONSCORECHANGED = u8"onScoreChanged";
	const std::string ONSCRIPTENGINEINIT = u8"onScriptEngineInit";
	const std::string ONSCRIPTENGINELOG = u8"onScriptEngineLog";
	const std::string ONSCRIPTENGINECMD = u8"onScriptEngineCmd";
	const std::string ONSCOREBOARDINIT = u8"onScoreboardInit";
#if (COMMERCIAL)
	const std::string ONMOBHURT = u8"onMobHurt";
	const std::string ONBLOCKCMD = u8"onBlockCmd";
	const std::string ONNPCCMD = u8"onNpcCmd";
	const std::string ONCOMMANDBLOCKUPDATE = u8"onCommandBlockUpdate";
#endif
};

#pragma pack(1)
struct Events {
	EventType type;	// �¼�����
	ActMode mode;	// ����ģʽ
	int result;		// �¼������ע��After�¼�ʱ����ֵ��Ч��
	void* data;		// ԭʼ����ָ��
};
#pragma pack()

//////////////// ����׷�ӹ���׷�ӵ��������ԣ������ƶ����̳к��¼�����ĩβ����֤������ ////////////////

struct ServerCmdEvent {
	char* cmd;	// ָ������
public:
	ServerCmdEvent() {
		memset(this, 0, sizeof(ServerCmdEvent));
	}
	void releaseAll() {
		if (cmd) {
			delete cmd;
			cmd = NULL;
		}
	}
};

struct ServerCmdOutputEvent {
	char* output;	// �����Ϣ
public:
	ServerCmdOutputEvent() {
		memset(this, 0, sizeof(ServerCmdOutputEvent));
	}
	void releaseAll() {
		if (output) {
			delete output;
			output = NULL;
		}
	}
};

struct PlayerEvent {
	char* playername;	// �������
	char* dimension;	// �������ά��
	Vec3 XYZ;			// �������λ��
	int dimensionid;	// �������ά��ID
	bool isstand;		// ����Ƿ������ڷ���/����
public:
	PlayerEvent() {
		memset(this, 0, sizeof(PlayerEvent));
	}
	void releaseAll() {
		if (playername) {
			delete playername;
			playername = NULL;
		}
		if (dimension) {
			delete dimension;
			dimension = NULL;
		}
	}
};

struct FormSelectEvent : PlayerEvent {
	char* uuid;			// ���uuid��Ϣ
	char* selected;		// ���ش���ѡ������Ϣ
	int formid;			// ��ID
	void* pplayer;		// ������������ָ��
public:
	FormSelectEvent() {
		memset(this, 0, sizeof(FormSelectEvent));
	}
	void releaseAll() {
		if (uuid) {
			delete uuid;
			uuid = NULL;
		}
		if (selected) {
			delete selected;
			selected = NULL;
		}
		((PlayerEvent*)this)->releaseAll();
	}
};

struct UseItemEvent : PlayerEvent {
	char* itemname;		// ��Ʒ����
	BPos3 position;		// ������������λ��
	short itemid;		// ��ƷID
	short itemaux;		// ��Ʒ����ֵ
	char* blockname;	// ������������
	short blockid;		// ��������ID
	void* pplayer;		// ������������ָ��
public:
	UseItemEvent() {
		memset(this, 0, sizeof(UseItemEvent));
	}
	void releaseAll() {
		if (itemname) {
			delete itemname;
			itemname = NULL;
		}
		if (blockname) {
			delete blockname;
			blockname = NULL;
		}
		((PlayerEvent*)this)->releaseAll();
	}
};

struct BlockEvent : PlayerEvent {
	char* blockname;	// ��������
	BPos3 position;		// ������������λ��
	short blockid;		// ����ID
	void* pplayer;		// ������������ָ��
public:
	BlockEvent() {
		memset(this, 0, sizeof(BlockEvent));
	}
	void releaseAll() {
		if (blockname) {
			delete blockname;
			blockname = NULL;
		}
		((PlayerEvent*)this)->releaseAll();
	}
};

struct PlacedBlockEvent : BlockEvent {
};

struct DestroyBlockEvent : BlockEvent {
};

struct StartOpenChestEvent : BlockEvent {
};

struct StartOpenBarrelEvent : BlockEvent {
};

struct StopOpenChestEvent : BlockEvent {
};

struct StopOpenBarrelEvent : BlockEvent {
};

struct SetSlotEvent : PlayerEvent {
	char* itemname;		// ��Ʒ����
	char* blockname;	// ��������
	BPos3 position;		// ������������λ��
	int itemcount;		// ��Ʒ����
	int slot;			// ��������λ��
	short itemaux;		// ��Ʒ����ֵ
	short blockid;		// ����ID
	short itemid;		// ��ƷID
	void* pplayer;		// ������������ָ��
public:
	SetSlotEvent() {
		memset(this, 0, sizeof(SetSlotEvent));
	}
	void releaseAll() {
		if (itemname) {
			delete itemname;
			itemname = NULL;
		}
		if (blockname) {
			delete blockname;
			blockname = NULL;
		}
		((PlayerEvent*)this)->releaseAll();
	}
};


struct ChangeDimensionEvent : PlayerEvent {
	void* pplayer;		// ������������ָ��
};

struct MobDieBaseEvent {
	char* mobname;		// ��������
	char* playername;	// ������֣���Ϊ������������Ӵ���Ϣ��
	char* dimension;	// �������ά�ȣ�������Ϣ��
	char* mobtype;		// ��������
	char* srcname;		// �˺�Դ����
	char* srctype;		// �˺�Դ����
	Vec3 XYZ;			// ��������λ��
	int dimensionid;	// ��������ά��ID
	int dmcase;			// �˺�����ԭ��ID
	bool isstand;		// ����Ƿ������ڷ���/���գ�������Ϣ��
public:
	MobDieBaseEvent() {
		memset(this, 0, sizeof(MobDieBaseEvent));
	}
	void releaseAll() {
		if (mobname) {
			delete mobname;
			mobname = NULL;
		}
		if (playername) {
			delete playername;
			playername = NULL;
		}
		if (dimension) {
			delete dimension;
			dimension = NULL;
		}
		if (mobtype) {
			delete mobtype;
			mobtype = NULL;
		}
		if (srcname) {
			delete srcname;
			srcname = NULL;
		}
		if (srctype) {
			delete srctype;
			srctype = NULL;
		}
	}
};

struct MobDieEvent : MobDieBaseEvent {
	void* pmob;		// ������������ָ��
};

struct MobHurtEvent : MobDieBaseEvent {
	char* dmtype;	// ������������
	float health;	// ����Ѫ��
	int dmcount;	// �������˾���ֵ
	void* pmob;		// �������������ָ��
public:
	MobHurtEvent() {
		memset(this, 0, sizeof(MobHurtEvent));
	}
	void releaseAll() {
		if (dmtype) {
			delete dmtype;
			dmtype = NULL;
		}
		((MobDieEvent*)this)->releaseAll();
	}
};

struct RespawnEvent : PlayerEvent {
	void* pplayer;		// ������������ָ��
};

struct ChatEvent {
	char* playername;	// ���������֣�����Ϊ������������鷢�ԣ�
	char* target;		// ����������
	char* msg;			// ���յ�����Ϣ
	char* chatstyle;	// ��������
public:
	ChatEvent() {
		memset(this, 0, sizeof(ChatEvent));
	}
	void releaseAll() {
		if (playername) {
			delete playername;
			playername = NULL;
		}
		if (target) {
			delete target;
			target = NULL;
		}
		if (msg) {
			delete msg;
			msg = NULL;
		}
		if (chatstyle) {
			delete chatstyle;
			chatstyle = NULL;
		}
	}
};

struct InputTextEvent : PlayerEvent {
	char* msg;		// ������ı�
	void* pplayer;	// ������������ָ��
public:
	InputTextEvent() {
		memset(this, 0, sizeof(InputTextEvent));
	}
	void releaseAll() {
		if (msg) {
			delete msg;
			msg = NULL;
		}
		((PlayerEvent*)this)->releaseAll();
	}
};

struct CommandBlockUpdateEvent : PlayerEvent {
	char* cmd;		// �������µ���ָ��
	char* actortype;// ʵ�����ͣ��������µ��ǷǷ��飬���Ӵ���Ϣ��
	BPos3 position;	// ���������λ��
	bool isblock;	// �Ƿ��Ƿ���
	void* pplayer;	// ������������ָ��
public:
	CommandBlockUpdateEvent() {
		memset(this, 0, sizeof(CommandBlockUpdateEvent));
	}
	void releaseAll() {
		if (cmd) {
			delete cmd;
			cmd = NULL;
		}
		if (actortype) {
			delete actortype;
			actortype = NULL;
		}
		((PlayerEvent*)this)->releaseAll();
	}
};

struct InputCommandEvent : PlayerEvent {
	char* cmd;	// ��������ָ��
	void* pplayer;	// ������������ָ��
public:
	InputCommandEvent() {
		memset(this, 0, sizeof(InputCommandEvent));
	}
	void releaseAll() {
		if (cmd) {
			delete cmd;
			cmd = NULL;
		}
		((PlayerEvent*)this)->releaseAll();
	}
};

struct BlockCmdEvent {
	char* cmd;		// ����ִ�е�ָ��
	char* name;		// ִ�����Զ�������
	char* dimension;// ���������ά��
	BPos3 position;	// ִ��������λ��
	int dimensionid;// ���������ά��ID
	int tickdelay;	// �����趨�ļ��ʱ��
	int type;		// ִ��������
public:
	BlockCmdEvent() {
		memset(this, 0, sizeof(BlockCmdEvent));
	}
	void releaseAll() {
		if (cmd) {
			delete cmd;
			cmd = NULL;
		}
		if (name) {
			delete name;
			name = NULL;
		}
		if (dimension) {
			delete dimension;
			dimension = NULL;
		}
	}
};

struct NpcCmdEvent {
	char* npcname;		// NPC����
	char* entity;		// NPCʵ���ʶ��
	char* dimension;	// NPC����ά��
	char* actions;		// ָ���б�
	Vec3 position;		// NPC����λ��
	int actionid;		// ѡ����
	int entityid;		// NPCʵ���ʶID
	int dimensionid;	// NPC����ά��ID
	void* pnpc;			// ���������npcָ��
	void* ptrigger;		// ���������������playerָ��
public:
	NpcCmdEvent() {
		memset(this, 0, sizeof(NpcCmdEvent));
	}
	void releaseAll() {
		if (npcname) {
			delete npcname;
			npcname = NULL;
		}
		if (entity) {
			delete entity;
			entity = NULL;
		}
		if (dimension) {
			delete dimension;
			dimension = NULL;
		}
		if (actions) {
			delete actions;
			actions = NULL;
		}
	}
};

struct LoadNameEvent {
	char* playername;	// �������
	char* uuid;			// ���uuid�ַ���
	char* xuid;			// ���xuid�ַ���
	char* ability;		// �������ֵ�б���ѡ����ҵ����ã�
	void* pplayer;		// ������������ָ��
public:
	LoadNameEvent() {
		memset(this, 0, sizeof(LoadNameEvent));
	}
	void releaseAll() {
		if (playername) {
			delete playername;
			playername = NULL;
		}
		if (uuid) {
			delete uuid;
			uuid = NULL;
		}
		if (xuid) {
			delete xuid;
			xuid = NULL;
		}
		if (ability) {
			delete ability;
			ability = NULL;
		}
	}
};

struct PlayerLeftEvent : LoadNameEvent {
};

struct MoveEvent : PlayerEvent {
	void* pplayer;		// ������������ָ��
};

struct AttackEvent : PlayerEvent
{
	char* actorname;		// ������ʵ������
	char* actortype;		// ������ʵ������
	Vec3 actorpos;			// ������ʵ������λ��
	void* pattacker;		// ������������ָ��
	void* pattackedentity;	// ���������������ʵ��ָ��
	int damagecause;		// �����˺�����
public:
	AttackEvent() {
		memset(this, 0, sizeof(AttackEvent));
	}
	void releaseAll() {
		if (actorname) {
			delete actorname;
			actorname = NULL;
		}
		if (actortype) {
			delete actortype;
			actortype = NULL;
		}
		((PlayerEvent*)this)->releaseAll();
	}
};

struct LevelExplodeEvent {
	char* entity;		// ��ը��ʵ���ʶ��������Ϊ�գ�
	char* blockname;	// ��ը������������Ϊ�գ�
	char* dimension;	// ��ը������ά��
	Vec3 position;		// ��ը������λ��
	int entityid;		// ��ը��ʵ���ʶID
	int dimensionid;	// ��ը������ά��ID
	float explodepower;	// ��ըǿ��
	short blockid;		// ��ը����ID
public:
	LevelExplodeEvent() {
		memset(this, 0, sizeof(LevelExplodeEvent));
	}
	void releaseAll() {
		if (entity) {
			delete entity;
			entity = NULL;
		}
		if (dimension) {
			delete dimension;
			dimension = NULL;
		}
	}
};

struct EquippedArmorEvent : PlayerEvent {
	char* itemname;		// ��Ʒ����
	int itemcount;		// ��Ʒ����
	int slot;			// ��������λ��
	short itemaux;		// ��Ʒ����ֵ
	short itemid;		// ��ƷID
	short slottype;		// �л���װ�����ͣ�0 - �����࣬1 - ������
	void* pplayer;		// ������������ָ��
public:
	EquippedArmorEvent() {
		memset(this, 0, sizeof(EquippedArmorEvent));
	}
	void releaseAll() {
		if (itemname) {
			delete itemname;
			itemname = NULL;
		}
	}
};

struct LevelUpEvent : PlayerEvent {
	void* pplayer;		// ������������ָ��
	int lv;				// ��ҵȼ�
public:
	LevelUpEvent() {
		memset(this, 0, sizeof(LevelUpEvent));
	}
	void releaseAll() {
		((PlayerEvent*)this)->releaseAll();
	}
};

struct ChestPairEvent {
	char* dimension;		// ��������ά��
	char* blockname;		// ������������
	char* targetblockname;	// ���Ʒ�������
	BPos3 position;			// ������������λ��
	BPos3 targetposition;	// ����Ŀ�귽������λ��
	int dimensionid;		// ��������ά��ID
	short blockid;			// ��������ID
	short targetblockid;	// ����Ŀ�귽��ID
public:
	ChestPairEvent() {
		memset(this, 0, sizeof(ChestPairEvent));
	}
	void releaseAll() {
		if (dimension) {
			delete dimension;
			dimension = NULL;
		}
		if (blockname) {
			delete blockname;
			blockname = NULL;
		}
		if (targetblockname) {
			delete targetblockname;
			targetblockname = NULL;
		}
	}
};
struct PistonPushEvent : ChestPairEvent {
	UINT8 direction;			// ����
public:
	PistonPushEvent() {
		memset(this, 0, sizeof(PistonPushEvent));
	}
};

struct MobSpawnCheckEvent {
	char* mobname;		// ��������
	char* dimension;	// ��������ά��
	char* mobtype;		// ��������
	Vec3 XYZ;			// ��������λ��
	int dimensionid;	// ��������ά��ID
	void* pmob;			// ����ָ��
public:
	MobSpawnCheckEvent() {
		memset(this, 0, sizeof(MobSpawnCheckEvent));
	}
	void releaseAll() {
		if (mobname) {
			delete mobname;
			mobname = NULL;
		}
		if (dimension) {
			delete dimension;
			dimension = NULL;
		}
		if (mobtype) {
			delete mobtype;
			mobtype = NULL;
		}
	}
};

struct PickUpItemEvent : PlayerEvent {
	char* itemname;		// ��Ʒ����
	short itemid;		// ��ƷID
	short itemaux;		// ��Ʒ����ֵ
	void* pplayer;		// ����������������
public:
	PickUpItemEvent() {
		memset(this, 0, sizeof(PickUpItemEvent));
	}
	void releaseAll() {
		if (itemname) {
			delete itemname;
			itemname = NULL;
		}
	}
};

struct ScoreChangedEvent {
	char* objectivename;		// �Ʒְ�����
	char* displayname;			// �Ʒְ���ʾ��
	__int64 scoreboardid;		// �Ʒְ�IDֵ
	int score;					// ����
public:
	ScoreChangedEvent() {
		memset(this, 0, sizeof(ScoreChangedEvent));
	}
	void releaseAll() {
		if (objectivename) {
			delete objectivename;
			objectivename = NULL;
		}
		if (displayname) {
			delete displayname;
			displayname = NULL;
		}
	}
};

struct ScriptEngineInitEvent {
	VA jsen;		// �ٷ��ű�����ָ��
public:
	ScriptEngineInitEvent() {
		memset(this, 0, sizeof(ScriptEngineInitEvent));
	}
};

struct ScriptEngineLogEvent : ScriptEngineInitEvent {
	char* log;		// �ٷ��ű�����log�����Ϣ
public:
	ScriptEngineLogEvent() {
		memset(this, 0, sizeof(ScriptEngineLogEvent));
	}
	void releaseAll() {
		if (log) {
			delete log;
			log = NULL;
		}
	}
};
struct ScriptEngineCmdEvent : ScriptEngineLogEvent {
};

struct ScoreboardInitEvent {
	VA scptr;		// ϵͳ�Ʒְ�ָ��
public:
	ScoreboardInitEvent() {
		memset(this, 0, sizeof(ScoreboardInitEvent));
	}
};