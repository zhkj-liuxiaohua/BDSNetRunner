#pragma once
#include <string>
#include "THook.h"
#include "RVAs.hpp"
#include <json/json.h>
#include "Component.h"

// 方块坐标结构体
#pragma region Blocks 定义方块结构体

struct BPos3 {
	INT32 x;
	INT32 y;
	INT32 z;
	std::string toJsonString() {
		char str[256];
		sprintf_s(str, "{\"x\":%d,\"y\":%d,\"z\":%d}", x, y, z);
		return std::string(str);
	}
};

struct BlockLegacy {
	// 获取方块名
	auto getFullName() const {				// IDA BlockLegacy::~BlockLegacy
		return (std::string&) * (__int64*)((__int64)this + 120);
	}
	// 获取方块ID号
	auto getBlockItemID() const {			// IDA BlockLegacy::BlockLegacy VanillaItems::serverInitCreativeItemsCallback Item::beginCreativeGroup "itemGroup.name.planks"
		short v3 = *(short*)((VA)this + 312);
		if (v3 < 0x100) {
			return v3;
		}
		return (short)(255 - v3);
	}
};

struct BlockPos {
	// 获取坐标数组头
	BPos3* getPosition() const {
		return reinterpret_cast<BPos3*>(reinterpret_cast<VA>(this));
	}
};

struct Block {
	// 获取源
	const BlockLegacy* getLegacyBlock() const {			// IDA LegacyStructureTemplate::_mapToProperty "waterlogged"
		return SYMCALL(BlockLegacy*, MSSYM_B1QE14getLegacyBlockB1AA5BlockB2AAE19QEBAAEBVBlockLegacyB2AAA2XZ, this);
	}
};

struct BlockActor {
	// 取方块位置
	BlockPos* getPosition() {				// IDA BlockActor::BlockActor
		return reinterpret_cast<BlockPos*>(reinterpret_cast<VA>(this) + 44);
	}
};

struct BlockSource {
	// 取方块
	Block* getBlock(const BlockPos* blkpos) {
		return SYMCALL(Block*,
			MSSYM_B1QA8getBlockB1AE11BlockSourceB2AAE13QEBAAEBVBlockB2AAE12AEBVBlockPosB3AAAA1Z,
			this, blkpos);
	}
	// 获取方块所处维度
	int getDimensionId() {					// IDA Dimension::onBlockChanged
		return *(int*)(*((VA*)this + 4) + 200);
	}
	// 获取指定范围内所有实体
	std::vector<VA*>* getEntities(VA *rect) {
		return SYMCALL(std::vector<VA*>*, MSSYM_MD5_73d55bcf0da8c45a15024daf84014ad7,
			this, 0, rect, 1);
	}
};

struct Dimension {
	// 获取方块源
	VA getBlockSource() {					// IDA Level::tickEntities
		return *((VA*)this + 10);
	}
};

struct Level {
	// 获取维度
	Dimension* getDimension(int did) {
		return SYMCALL(Dimension*, MSSYM_B1QE12getDimensionB1AA5LevelB2AAE17QEBAPEAVDimensionB2AAA1VB2QDE11AutomaticIDB1AE10VDimensionB2AAA1HB3AAAA1Z,
			this, did);
	}
};

#pragma endregion


#pragma region Actors 定义实体结构体

struct MCUUID {
	// 取uuid字符串
	std::string toString() {
		std::string s;
		SYMCALL(std::string&, MSSYM_MD5_40e8abf6eb08f7ee446159cdd0a7f283, this, &s);
		return s;
	}
};

// 玩家坐标结构体
struct Vec3 {
	float x;
	float y;
	float z;

	std::string toJsonString() {
		char str[256];
		sprintf_s(str, "{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}", x, y, z);
		return std::string(str);
	}
};

// 区域范围结构体
struct AABB {
	Vec3 min;
	Vec3 max;
	bool empty;

	// 重设两点
	void set(float x1, float y1, float z1, float x2, float y2, float z2) {
		min.x = std::min<float>(x1, x2);
		min.y = std::min<float>(y1, y2);
		min.z = std::min<float>(z1, z2);
		max.x = std::max<float>(x1, x2);
		max.y = std::max<float>(y1, y2);
		max.z = std::max<float>(z1, z2);
		empty = (min.x == 0.0 && min.y == 0.0 && min.z == 0.0 &&
			max.x == 0.0 && max.y == 0.0 && max.z == 0.0);
	}
	// 从两点间获取一个区域
	bool fromPoints(Vec3 *a, Vec3 *b) {
		if (!a || !b)
			return false;
		min.x = std::min<float>(a->x, b->x);
		min.y = std::min<float>(a->y, b->y);
		min.z = std::min<float>(a->z, b->z);
		max.x = std::max<float>(a->x, b->x);
		max.y = std::max<float>(a->y, b->y);
		max.z = std::max<float>(a->z, b->z);
		empty = (min.x == 0.0 && min.y == 0.0 && min.z == 0.0 &&
			max.x == 0.0 && max.y == 0.0 && max.z == 0.0);
		return true;
	}
};

// 玩家转角结构体
struct Vec2 {
	float x;
	float y;
};

struct Actor {
#pragma region 通用属性
	// 导出API，获取穿戴信息，只读
	static std::string sgetArmorContainer(Actor*);
	// 导出API，获取攻击力
	static std::string sgetAttack(Actor*);
	// 导出API，设置攻击力
	static bool ssetAttack(Actor*, const char*);
	// 导出API，获取碰撞箱
	static std::string sgetCollisionBox(Actor*);
	// 导出API，设置碰撞箱
	static bool ssetCollisionBox(Actor*, const char*);
	// damage_sensor，伤害指示器，复杂类型，略
	// equipment，装备掉率列表，复杂类型，略
	// equippable，允许装备内容列表，复杂类型，略
	// explode，爆炸力，复杂类型，略
	
	// 导出API，获取主副手装备，只读
	static std::string sgetHandContainer(Actor*);
	// healable，补品列表，复杂类型，略

	// 导出API，获取生命值
	static std::string sgetHealth(Actor*);
	// 导出API，设置生命值
	static bool ssetHealth(Actor*, const char*);
	// interact，玩家与实体的交互动作，复杂类型，略
	// inventory，实体背包类型，复杂类型，略

	// 导出API，获取实体容器列表，只读
	static std::string sgetInventoryContainer(Actor*);
	// lookat，敌对关注，复杂类型，略
	// nameable，自定义名称相关属性，复杂类型，略
	
	// 导出API，获取名称信息
	static std::string sgetName(Actor*);
	// 导出API，设置名称信息，是否一直显示
	static bool ssetName(Actor*, const char*, bool);
	// 导出API，获取三维坐标信息
	static std::string sgetPosition(Actor*);
	// 导出API，设置三维坐标
	static bool ssetPosition(Actor*, const char*);
	// 导出API，获取实体转角信息
	static std::string sgetRotation(Actor*);
	// 导出API，设置实体转角
	static bool ssetRotation(Actor*, const char*);
	// shooter，定义实体发射飞行物属性，需要实体具有projectile组件，复杂类型，略
	// spawn_entity，定义实体诞生其它新实体的属性（如鸡等），复杂类型，略
	// teleport，定义实体自随机传送属性（如末影人等），复杂类型，略
	// tick_world，实体可用更新域、于世界的刷新行为等，复杂类型，略
	
	// 导出API，获取维度ID，只读
	static int sgetDimensionId(Actor*);
	// 导出API，获取实体类型ID，只读
	static int sgetEntityTypeId(Actor*);
	// 导出API，获取查询ID，只读
	static VA sgetUniqueID(Actor*);
	// 导出API，移除该实体
	static bool sremove(Actor*);
	// 导出API，模拟该实体产生一个源自另一实体的伤害
	static bool shurt(Actor*, Actor*, ActorDamageCause, int, bool, bool);
	// 导出API，根据查询ID反查实体指针
	static Actor* sgetfromUniqueID(VA);
	// 导出API，查询指定维度指定坐标范围内所有实体
	static std::vector<VA*>* sgetEntities(int, float, float, float, float, float, float);
#pragma endregion

	// 取方块源
	BlockSource* getRegion() {						// IDA
		return (BlockSource*)*((VA*)this + 105);
	}

	// 获取生物名称信息
	std::string getNameTag() {
		return SYMCALL(std::string&,
			MSSYM_MD5_7044ab83168b0fd345329e6566fd47fd,
			this);
	}

	// 获取生物当前所处维度ID
	int getDimensionId() {
		int dimensionId;
		SYMCALL(int&,
			MSSYM_B1QE14getDimensionIdB1AA5ActorB2AAA4UEBAB1QA2AVB2QDE11AutomaticIDB1AE10VDimensionB2AAA1HB2AAA2XZ,
			this, &dimensionId);
		return dimensionId;
	}

	// 是否悬空
	const BYTE isStand() {				// IDA MovePlayerPacket::MovePlayerPacket
		return *reinterpret_cast<BYTE*>(reinterpret_cast<VA>(this) + 448);
	}

	// 获取生物当前所在坐标
	Vec3* getPos() {
		return SYMCALL(Vec3*,
			MSSYM_B1QA6getPosB1AA5ActorB2AAE12UEBAAEBVVec3B2AAA2XZ, this);
	}

	// 获取生物类型
	std::string getTypeName() {
		std::string actor_typename;					// IDA ActorCommandOrigin::getName CommandUtils::getActorName
		SYMCALL(std::string&,
			MSSYM_MD5_41a18e1578312643b066a562efefb36a,
			&actor_typename, this);
		return actor_typename;
	}

	// 获取实体类型
	int getEntityTypeId() {
		return sgetEntityTypeId(this);
	}

	// 获取查询用ID
	VA* getUniqueID() {
		return SYMCALL(VA*, MSSYM_B1QE11getUniqueIDB1AA5ActorB2AAE21QEBAAEBUActorUniqueIDB2AAA2XZ, this);
	}
	// 获取实体名称
	std::string getEntityTypeName() {
		std::string en_name;
		SYMCALL(std::string&,
			MSSYM_MD5_af48b8a1869a49a3fb9a4c12f48d5a68,
			&en_name, getEntityTypeId());
		return en_name;
	}
};

struct Mob : Actor {
	// 获取装备容器
	VA getArmor() {					// IDA Mob::addAdditionalSaveData
		return VA(this) + 1472;
	}
	// 获取手头容器
	VA getHands() {
		return VA(this) + 1480;		// IDA Mob::readAdditionalSaveData
	}
	// 获取地图信息
	VA getLevel() {					// IDA Mob::die
		return *((VA*)((VA)this + 856));
	}
};
struct Player : Mob {
#pragma region 通用属性
	// 导出API，获取热键装备列表，只读
	static std::string sgetHotbarContainer(Player*);
	// 导出API，获取玩家uuid，只读
	static std::string sgetUuid(Player*);
	// 导出API，获取玩家网络IP和端口，只读
	static std::string sgetIPPort(Player*);
	// 导出API，增加玩家等级
	static void saddLevel(Player*, int);
#pragma endregion

	// 取uuid
	MCUUID* getUuid() {				// IDA ServerNetworkHandler::_createNewPlayer
		return (MCUUID*)((char*)this + 2800);
	}

	// 根据地图信息获取玩家xuid
	std::string& getXuid(VA level) {
		return SYMCALL(std::string&, MSSYM_MD5_337bfad553c289ba4656ac43dcb60748,
			level, (char*)this + 2800);
	}

	// 重设服务器玩家名
	void reName(std::string name) {
		SYMCALL(void, MSSYM_B1QA7setNameB1AA6PlayerB2AAA9UEAAXAEBVB2QDA5basicB1UA6stringB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB2AAA1VB2QDA9allocatorB1AA1DB1AA12B2AAA3stdB3AAAA1Z,
			this, name);
	}

	// 获取网络标识符
	VA getNetId() {
		return (VA)this + 2512;		// IDA ServerPlayer::setPermissions
	}
	// 获取背包
	VA getSupplies() {				// IDA Player::add
		return *(VA*)(*((VA*)this + 378) + 176);
	}

	// 添加一个物品
	void addItem(VA item) {
		SYMCALL(VA, MSSYM_B1QA3addB1AA6PlayerB2AAA4UEAAB1UE14NAEAVItemStackB3AAAA1Z, this, item);
	}

	// 更新所有物品列表
	void updateInventory() {
		VA itm = (VA)this + 4568;				// IDA Player::drop
		SYMCALL(VA, MSSYM_B1QE23forceBalanceTransactionB1AE27InventoryTransactionManagerB2AAA7QEAAXXZ, itm);
	}
	// 发送数据包
	VA sendPacket(VA pkt) {
		return SYMCALL(VA, MSSYM_B1QE17sendNetworkPacketB1AE12ServerPlayerB2AAE15UEBAXAEAVPacketB3AAAA1Z, this, pkt);
	}
};

#pragma endregion

#pragma region Items 定义物品结构体

struct Item {
};

struct ItemStackBase {
	VA vtable;
	VA mItem;
	VA mUserData;
	VA mBlock;
	short mAuxValue;
	char mCount;
	char mValid;
	char unk[4]{ 0 };
	VA mPickupTime;
	char mShowPickUp;
	char unk2[7]{ 0 };
	std::vector<VA*> mCanPlaceOn;
	VA mCanPlaceOnHash;
	std::vector<VA*> mCanDestroy;
	VA mCanDestroyHash;
	VA mBlockingTick;
	ItemStackBase* mChargedItem;
	VA uk;
public:
	bool getFromId(short id, short aux, char count) {
		memcpy(this, SYM_POINT(void, MSSYM_B1QA5EMPTYB1UA4ITEMB1AA9ItemStackB2AAA32V1B1AA1B), 0x90);
		bool ret = SYMCALL(bool, MSSYM_B2QUA7setItemB1AE13ItemStackBaseB2AAA4IEAAB1UA2NHB1UA1NB1AA1Z, this, id, false);
		mCount = count;
		mAuxValue = aux;
		mValid = true;
		return ret;
	}
};

struct ItemStack : ItemStackBase {
	// 取物品ID
	short getId() {
		return SYMCALL(short,
			MSSYM_B1QA5getIdB1AE13ItemStackBaseB2AAA7QEBAFXZ,
			this);
	}
	// 取物品特殊值
	short getAuxValue() {
		return SYMCALL(short,
			MSSYM_B1QE11getAuxValueB1AE13ItemStackBaseB2AAA7QEBAFXZ,
			this);
	}
	// 取物品名称
	std::string getName() {
		std::string str;				// IDA ItemStackBase::getName
		SYMCALL(__int64,
			MSSYM_MD5_6d581a35d7ad70fd364b60c3ebe93394,
			this, &str);
		return str;
	}
	// 取物品命名空间标识
	std::string getRawNameId() {
		std::string str;
		SYMCALL(VA, MSSYM_MD5_2f9d68ca736b0da0c26f063f568898bc,
			this, &str);
		return str;
	}
	// 取容器内数量
	int getStackSize() {			// IDA ContainerModel::networkUpdateItem
		return *((char*)this + 34);
	}
	// 判断是否空容器
	bool isNull() {
		return SYMCALL(bool,
			MSSYM_B1QA6isNullB1AE13ItemStackBaseB2AAA4QEBAB1UA3NXZ,
			this);
	}
};


struct LevelContainerModel {
	// 取开容者
	Player* getPlayer() {
		return ((Player**)this)[26];
	}
};

#pragma endregion

#pragma region Packets 定义数据包结构体

struct TextPacket {
	char filler[0xC8];
	// 取输入文本
	std::string toString() {			// IDA ServerNetworkHandler::handle
		std::string str = std::string(*(std::string*)((VA)this + 80));
		return str;
	}
};

struct CommandRequestPacket {
	char filler[0x90];
	// 取命令文本
	std::string toString() {			// IDA ServerNetworkHandler::handle
		std::string str = std::string(*(std::string*)((VA)this + 40));
		return str;
	}
};

struct ModalFormRequestPacket {
	char filler[0x48];
};

struct ModalFormResponsePacket {
	// 取发起表单ID
	UINT getFormId() {
		return *(UINT*)((VA)this + 40);
	}
	// 取选择序号
	std::string getSelectStr() {
		std::string x = *(std::string*)((VA)this + 48);
		VA l = x.length();
		if (x.c_str()[l - 1] == '\n') {
			return l > 1 ? x.substr(0, l - 1) :
				x;
		}
		return x;
	}
};

#pragma endregion