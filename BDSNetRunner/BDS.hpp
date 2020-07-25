#pragma once
#include <string>
#include "THook.h"
#include "RVAs.hpp"
#include <json/json.h>

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
		return (std::string&) * (__int64*)((__int64)this + 112);
	}
	// 获取方块ID号
	auto getBlockItemID() const {			// IDA VanillaItems::initCreativeItemsCallback Item::beginCreativeGroup "itemGroup.name.planks"
		short v3 = *(short*)((VA)this + 268);
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
		return (BlockLegacy*)(**(VA**)((VA)this + 16));
	}
};

struct BlockActor {
	// 取方块
	Block* getBlock() {
		return *reinterpret_cast<Block**>(reinterpret_cast<VA>(this) + 16);
	}
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
		return *(int*)(*((VA*)this + 4) + 192);
	}
};

struct Dimension {
	// 获取方块源
	VA getBlockSouce() {					// IDA Level::tickEntities
		return *((VA*)this + 9);
	}
};

struct Level {
	// 获取维度
	Dimension* getDimension(int did) {
		return SYMCALL(Dimension*, MSSYM_B1QE12getDimensionB1AA5LevelB2AAE17QEBAPEAVDimensionB2AAA1VB2QDE11AutomaticIDB1AE10VDimensionB2AAA1HB3AAAA1Z,
			this, did);
	}
	// 获取计分板
	VA getScoreBoard() {				// IDA Level::removeEntityReferences
		return *(VA*)((VA)this + 8280);
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


struct MobEffectInstance {
	char fill[0x1C];
};

struct Actor {
	// 取方块源
	BlockSource* getRegion() {
		return *reinterpret_cast<BlockSource**>(reinterpret_cast<VA>(this) + 414 * sizeof(void*));
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
		return *reinterpret_cast<BYTE*>(reinterpret_cast<VA>(this) + 416);
	}

	// 获取生物当前所在坐标
	Vec3* getPos() {
		return SYMCALL(Vec3*,
			MSSYM_B1QA6getPosB1AA5ActorB2AAE12UEBAAEBVVec3B2AAA2XZ, this);
	}

	// 获取生物类型
	std::string getTypeName() {
		std::string actor_typename;
		SYMCALL(std::string&,
			MSSYM_MD5_01064f7d893d9f9ef50acf1f931d1d79,
			&actor_typename, this);
		return actor_typename;
	}

	// 获取实体类型
	int getEntityTypeId() {
		return SYMCALL(int,
			MSSYM_B1QE15getEntityTypeIdB1AA5ActorB2AAA4UEBAB1QE12AW4ActorTypeB2AAA2XZ,
			this);
		//		if (t == 1)		// 未知类型，可能是玩家
		//			return 319;
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

	// 更新属性
	VA updateAttrs() {
		return SYMCALL(VA, MSSYM_B2QUE18sendDirtyActorDataB1AA5ActorB2AAA7QEAAXXZ, this);
	}
	// 添加一个状态
	VA addEffect(VA ef) {
		return SYMCALL(VA, MSSYM_B1QA9addEffectB1AA5ActorB2AAE26QEAAXAEBVMobEffectInstanceB3AAAA1Z, this, ef);
	}
};

struct Mob : Actor {
	// 获取状态列表
	std::vector<MobEffectInstance>* getEffects() {					// IDA Mob::addAdditionalSaveData
		return (std::vector<MobEffectInstance>*)((VA*)this + 152);
	}

	// 获取装备容器
	VA getArmor() {					// IDA Mob::addAdditionalSaveData
		return VA(this) + 1400;
	}
	// 获取手头容器
	VA getHands() {
		return VA(this) + 1408;		// IDA Mob::readAdditionalSaveData
	}
	// 保存当前副手至容器
	VA saveOffhand(VA hlist) {
		return SYMCALL(VA, MSSYM_B1QE11saveOffhandB1AA3MobB2AAA4IEBAB1QA2AVB2QDA6uniqueB1UA3ptrB1AA8VListTagB2AAA1UB2QDA7defaultB1UA6deleteB1AA8VListTagB3AAAA3stdB3AAAA3stdB2AAA2XZ,
			this, hlist);
	}
	// 获取地图信息
	VA getLevel() {					// IDA Mob::die
		return *((VA*)((VA)this + 816));
	}
};
struct Player : Mob {
public:
	// 取uuid
	MCUUID* getUuid() {				// IDA ServerNetworkHandler::_createNewPlayer
		return (MCUUID*)((char*)this + 2720);
	}

	// 根据地图信息获取玩家xuid
	std::string& getXuid(VA level) {
		return SYMCALL(std::string&, MSSYM_MD5_337bfad553c289ba4656ac43dcb60748,
			level, (char*)this + 2720);
	}

	// 重设服务器玩家名
	void reName(std::string name) {
		SYMCALL(void, MSSYM_B1QA7setNameB1AA6PlayerB2AAA9UEAAXAEBVB2QDA5basicB1UA6stringB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB2AAA1VB2QDA9allocatorB1AA1DB1AA12B2AAA3stdB3AAAA1Z,
			this, name);
	}

	// 获取网络标识符
	VA getNetId() {
		return (VA)this + 2432;		// IDA ServerPlayer::setPermissions
	}

	VA getContainerManager() {
		return (VA)this + 2912;		// IDA Player::setContainerManager
	}
	// 获取背包
	VA getSupplies() {				// IDA Player::add
		return *(VA*)(*((VA*)this + 366) + 176);
	}
	// 获取末影箱
	VA getEnderChestContainer() {
		return SYMCALL(VA, MSSYM_B1QE22getEnderChestContainerB1AA6PlayerB2AAE27QEAAPEAVEnderChestContainerB2AAA2XZ, this);
	}
	// 设置一个装备
	VA setArmor(int i, VA item) {
		return SYMCALL(VA, MSSYM_B1QA8setArmorB1AE12ServerPlayerB2AAE16UEAAXW4ArmorSlotB2AAE13AEBVItemStackB3AAAA1Z, this, i, item);
	}
	// 设置副手
	VA setOffhandSlot(VA item) {
		return SYMCALL(VA, MSSYM_B1QE14setOffhandSlotB1AA6PlayerB2AAE18UEAAXAEBVItemStackB3AAAA1Z, this, item);
	}
	// 添加一个物品
	void addItem(VA item) {
		SYMCALL(VA, MSSYM_B1QA3addB1AA6PlayerB2AAA4UEAAB1UE14NAEAVItemStackB3AAAA1Z, this, item);
	}
	// 获取当前选中的框位置
	int getSelectdItemSlot() {			// IDA Player::getSelectedItem
		VA v1 = *((VA*)this + 366);
		return *(unsigned int*)(v1 + 16);
	}
	// 获取当前物品
	VA getSelectedItem() {
		VA x = SYMCALL(VA, MSSYM_B1QE15getSelectedItemB1AA6PlayerB2AAE17QEBAAEBVItemStackB2AAA2XZ,
			this);
		return x;
	}
	// 获取游戏时命令权限
	char getPermission() {						// IDA ServerPlayer::setPermissions
		return **(char**)((VA)this + 2112);
	}
	// 设置游戏时命令权限
	void setPermission(char m) {
		SYMCALL(void, MSSYM_B1QE14setPermissionsB1AE12ServerPlayerB2AAE29UEAAXW4CommandPermissionLevelB3AAAA1Z,
			this, m);
	}
	// 获取游戏时游玩权限
	char getPermissionLevel() {						// IDA Abilities::setPlayerPermissions
		return (*(char**)((VA)this + 2112))[1];
	}
	// 设置游戏时游玩权限
	void setPermissionLevel(char m) {
		SYMCALL(void, MSSYM_B1QE20setPlayerPermissionsB1AA9AbilitiesB2AAE28QEAAXW4PlayerPermissionLevelB3AAAA1Z,
			(VA)this + 2112, m);
	}
	// 更新所有物品列表
	void updateInventory() {
		VA itm = (VA)this + 4472;				// IDA Player::drop
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
	VA save() {
		VA* cp = new VA[8]{ 0 };
		return SYMCALL(VA, MSSYM_MD5_e02d5851c93a43bfe24a4396ecb87cde, this, cp);
	}
#if (COMMERCIAL)
	Json::Value toJson() {
		VA t = save();
		Json::Value jv = (*(Tag**)t)->toJson();
		(*(Tag**)t)->clearAll();
		*(VA*)t = 0;
		delete (VA*)t;
		return jv;
	}
	void fromJson(Json::Value& jv) {
		VA t = Tag::fromJson(jv);
		SYMCALL(VA, MSSYM_B1QA7fromTagB1AA9ItemStackB2AAA2SAB1QA3AV1B1AE15AEBVCompoundTagB3AAAA1Z, this, *(VA*)t);
		(*(Tag**)t)->clearAll();
		*(VA*)t = 0;
		delete (VA*)t;
	}
	void fromTag(VA t) {
		SYMCALL(VA, MSSYM_B1QA7fromTagB1AA9ItemStackB2AAA2SAB1QA3AV1B1AE15AEBVCompoundTagB3AAAA1Z, this, t);
	}
#endif
	bool getFromId(short id, short aux, char count) {
		memcpy(this, SYM_POINT(void, MSSYM_B1QA5EMPTYB1UA4ITEMB1AA9ItemStackB2AAA32V1B1AA1B), 0x90);
		bool ret = SYMCALL(bool, MSSYM_B2QUA7setItemB1AE13ItemStackBaseB2AAA4IEAAB1UA2NHB1AA1Z, this, id);
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
		std::string str;
		SYMCALL(__int64,
			MSSYM_MD5_6d581a35d7ad70fd364b60c3ebe93394,
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


struct Container {
	VA vtable;
	// 获取容器内所有物品
	VA getSlots(std::vector<ItemStack*>* s) {
		return SYMCALL(VA, MSSYM_B1QA8getSlotsB1AA9ContainerB2AAA4UEBAB1QA2BVB2QDA6vectorB1AE13PEBVItemStackB2AAA1VB2QDA9allocatorB1AE13PEBVItemStackB3AAAA3stdB3AAAA3stdB2AAA2XZ,
			this, s);
	}

};

struct SimpleContainer : Container {
	// 获取一个指定框内物品
	ItemStack* getItem(int slot) {
		return SYMCALL(ItemStack*, MSSYM_B1QA7getItemB1AE15SimpleContainerB2AAE17UEBAAEBVItemStackB2AAA1HB1AA1Z, this, slot);
	}
	// 设置一个指定框内的物品
	VA setItem(int slot, ItemStack* item) {
		return SYMCALL(VA, MSSYM_B1QA7setItemB1AE15SimpleContainerB2AAE19UEAAXHAEBVItemStackB3AAAA1Z,
			this, slot, item);
	}
};

struct FillingContainer : Container {
	// 格式化容器所有物品至tag
	VA save(VA tag) {
		return SYMCALL(VA, MSSYM_B1QA4saveB1AE16FillingContainerB2AAA4QEAAB1QA2AVB2QDA6uniqueB1UA3ptrB1AA8VListTagB2AAA1UB2QDA7defaultB1UA6deleteB1AA8VListTagB3AAAA3stdB3AAAA3stdB2AAA2XZ,
			this, tag);
	}
	// 设置容器中指定位置的物品
	VA setItem(int i, VA item) {
		return SYMCALL(VA, MSSYM_B1QA7setItemB1AE16FillingContainerB2AAE19UEAAXHAEBVItemStackB3AAAA1Z, this, i, item);
	}
};

struct IContainerManager {
	VA vtable;
};

struct PlayerInventoryProxy {
	VA vtable;
	VA vtable2;
	int mSelected;
	char uk1[4];
	ItemStack mInfiniteItem;
	char mSelectedContainerId;
	char uk2[7];
	VA mInventory;
	std::vector<ItemStack> mComplexItems;
	std::weak_ptr<VA> mHudContainerManager;
};

struct ContainerItemStack
	:ItemStack {

};

struct ContainerManagerModel {
	// 取开容者
	Player* getPlayer() {				// IDA ContainerManagerModel::ContainerManagerModel
		return *reinterpret_cast<Player**>(reinterpret_cast<VA>(this) + 8);
	}
};
struct LevelContainerManagerModel
	:ContainerManagerModel {
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