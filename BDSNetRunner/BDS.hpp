#pragma once
#include <string>
#include "THook.h"
#include "RVAs.hpp"
#include <json/json.h>

// ��������ṹ��
#pragma region Blocks ���巽��ṹ��

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
	// ��ȡ������
	auto getFullName() const {				// IDA BlockLegacy::~BlockLegacy
		return (std::string&) * (__int64*)((__int64)this + 112);
	}
	// ��ȡ����ID��
	auto getBlockItemID() const {			// IDA VanillaItems::initCreativeItemsCallback Item::beginCreativeGroup "itemGroup.name.planks"
		short v3 = *(short*)((VA)this + 268);
		if (v3 < 0x100) {
			return v3;
		}
		return (short)(255 - v3);
	}
};

struct BlockPos {
	// ��ȡ��������ͷ
	BPos3* getPosition() const {
		return reinterpret_cast<BPos3*>(reinterpret_cast<VA>(this));
	}
};

struct Block {
	// ��ȡԴ
	const BlockLegacy* getLegacyBlock() const {			// IDA LegacyStructureTemplate::_mapToProperty "waterlogged"
		return (BlockLegacy*)(**(VA**)((VA)this + 16));
	}
};

struct BlockActor {
	// ȡ����
	Block* getBlock() {
		return *reinterpret_cast<Block**>(reinterpret_cast<VA>(this) + 16);
	}
	// ȡ����λ��
	BlockPos* getPosition() {				// IDA BlockActor::BlockActor
		return reinterpret_cast<BlockPos*>(reinterpret_cast<VA>(this) + 44);
	}
};

struct BlockSource {
	// ȡ����
	Block* getBlock(const BlockPos* blkpos) {
		return SYMCALL(Block*,
			MSSYM_B1QA8getBlockB1AE11BlockSourceB2AAE13QEBAAEBVBlockB2AAE12AEBVBlockPosB3AAAA1Z,
			this, blkpos);
	}
	// ��ȡ��������ά��
	int getDimensionId() {					// IDA Dimension::onBlockChanged
		return *(int*)(*((VA*)this + 4) + 192);
	}
	// ��ȡָ����Χ������ʵ��
	std::vector<VA*>* getEntities(VA *rect) {
		return SYMCALL(std::vector<VA*>*, MSSYM_MD5_73d55bcf0da8c45a15024daf84014ad7,
			this, 0, rect, 1);
	}
};

struct Dimension {
	// ��ȡ����Դ
	VA getBlockSource() {					// IDA Level::tickEntities
		return *((VA*)this + 9);
	}
};

struct Level {
	// ��ȡά��
	Dimension* getDimension(int did) {
		return SYMCALL(Dimension*, MSSYM_B1QE12getDimensionB1AA5LevelB2AAE17QEBAPEAVDimensionB2AAA1VB2QDE11AutomaticIDB1AE10VDimensionB2AAA1HB3AAAA1Z,
			this, did);
	}
	// ��ȡ�Ʒְ�
	VA getScoreBoard() {				// IDA Level::removeEntityReferences
		return *(VA*)((VA)this + 8280);
	}
};

#pragma endregion


#pragma region Actors ����ʵ��ṹ��

struct MCUUID {
	// ȡuuid�ַ���
	std::string toString() {
		std::string s;
		SYMCALL(std::string&, MSSYM_MD5_40e8abf6eb08f7ee446159cdd0a7f283, this, &s);
		return s;
	}
};

// �������ṹ��
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

// ����Χ�ṹ��
struct AABB {
	Vec3 min;
	Vec3 max;
	bool empty;

	// ��������
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
	// ��������ȡһ������
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

// ���ת�ǽṹ��
struct Vec2 {
	float x;
	float y;
};

struct MobEffectInstance {
	char fill[0x1C];
};

struct Actor {
#pragma region ͨ������
	// ����API����ȡ������Ϣ��ֻ��
	static std::string sgetArmorContainer(Actor*);
	// ����API����ȡ������
	static std::string sgetAttack(Actor*);
	// ����API�����ù�����
	static bool ssetAttack(Actor*, const char*);
	// ����API����ȡ��ײ��
	static std::string sgetCollisionBox(Actor*);
	// ����API��������ײ��
	static bool ssetCollisionBox(Actor*, const char*);
	// damage_sensor���˺�ָʾ�����������ͣ���
	// equipment��װ�������б��������ͣ���
	// equippable������װ�������б��������ͣ���
	// explode����ը�����������ͣ���
	
	// ����API����ȡ������װ����ֻ��
	static std::string sgetHandContainer(Actor*);
	// healable����Ʒ�б��������ͣ���

	// ����API����ȡ����ֵ
	static std::string sgetHealth(Actor*);
	// ����API����������ֵ
	static bool ssetHealth(Actor*, const char*);
	// interact�������ʵ��Ľ����������������ͣ���
	// inventory��ʵ�屳�����ͣ��������ͣ���

	// ����API����ȡʵ�������б�ֻ��
	static std::string sgetInventoryContainer(Actor*);
	// lookat���жԹ�ע���������ͣ���
	// nameable���Զ�������������ԣ��������ͣ���
	
	// ����API����ȡ������Ϣ
	static std::string sgetName(Actor*);
	// ����API������������Ϣ���Ƿ�һֱ��ʾ
	static bool ssetName(Actor*, const char*, bool);
	// ����API����ȡ��ά������Ϣ
	static std::string sgetPosition(Actor*);
	// ����API��������ά����
	static bool ssetPosition(Actor*, const char*);
	// ����API����ȡʵ��ת����Ϣ
	static std::string sgetRotation(Actor*);
	// ����API������ʵ��ת��
	static bool ssetRotation(Actor*, const char*);
	// shooter������ʵ�巢����������ԣ���Ҫʵ�����projectile������������ͣ���
	// spawn_entity������ʵ�嵮��������ʵ������ԣ��缦�ȣ����������ͣ���
	// teleport������ʵ��������������ԣ���ĩӰ�˵ȣ����������ͣ���
	// tick_world��ʵ����ø������������ˢ����Ϊ�ȣ��������ͣ���
	
	// ����API����ȡά��ID��ֻ��
	static int sgetDimensionId(Actor*);
	// ����API����ȡʵ������ID��ֻ��
	static int sgetEntityTypeId(Actor*);
	// ����API����ȡ��ѯID��ֻ��
	static VA sgetUniqueID(Actor*);
	// ����API���Ƴ���ʵ��
	static bool sremove(Actor*);
	// ����API�����ݲ�ѯID����ʵ��ָ��
	static Actor* sgetfromUniqueID(VA);
	// ����API����ѯָ��ά��ָ�����귶Χ������ʵ��
	static std::vector<VA*>* sgetEntities(int, float, float, float, float, float, float);
#pragma endregion

	// ȡ����Դ
	BlockSource* getRegion() {
		return *reinterpret_cast<BlockSource**>(reinterpret_cast<VA>(this) + 414 * sizeof(void*));
	}

	// ��ȡ����������Ϣ
	std::string getNameTag() {
		return SYMCALL(std::string&,
			MSSYM_MD5_7044ab83168b0fd345329e6566fd47fd,
			this);
	}

	// ��ȡ���ﵱǰ����ά��ID
	int getDimensionId() {
		int dimensionId;
		SYMCALL(int&,
			MSSYM_B1QE14getDimensionIdB1AA5ActorB2AAA4UEBAB1QA2AVB2QDE11AutomaticIDB1AE10VDimensionB2AAA1HB2AAA2XZ,
			this, &dimensionId);
		return dimensionId;
	}

	// �Ƿ�����
	const BYTE isStand() {				// IDA MovePlayerPacket::MovePlayerPacket
		return *reinterpret_cast<BYTE*>(reinterpret_cast<VA>(this) + 416);
	}

	// ��ȡ���ﵱǰ��������
	Vec3* getPos() {
		return SYMCALL(Vec3*,
			MSSYM_B1QA6getPosB1AA5ActorB2AAE12UEBAAEBVVec3B2AAA2XZ, this);
	}

	// ��ȡ��������
	std::string getTypeName() {
		std::string actor_typename;
		SYMCALL(std::string&,
			MSSYM_MD5_01064f7d893d9f9ef50acf1f931d1d79,
			&actor_typename, this);
		return actor_typename;
	}

	// ��ȡʵ������
	int getEntityTypeId() {
		return SYMCALL(int,
			MSSYM_B1QE15getEntityTypeIdB1AA5ActorB2AAA4UEBAB1QE12AW4ActorTypeB2AAA2XZ,
			this);
		//		if (t == 1)		// δ֪���ͣ����������
		//			return 319;
	}

	// ��ȡ��ѯ��ID
	VA* getUniqueID() {
		return SYMCALL(VA*, MSSYM_B1QE11getUniqueIDB1AA5ActorB2AAE21QEBAAEBUActorUniqueIDB2AAA2XZ, this);
	}

	// ��ȡʵ������
	std::string getEntityTypeName() {
		std::string en_name;
		SYMCALL(std::string&,
			MSSYM_MD5_af48b8a1869a49a3fb9a4c12f48d5a68,
			&en_name, getEntityTypeId());
		return en_name;
	}

	// ��������
	VA updateAttrs() {
		return SYMCALL(VA, MSSYM_B2QUE18sendDirtyActorDataB1AA5ActorB2AAA7QEAAXXZ, this);
	}
	// ���һ��״̬
	VA addEffect(VA ef) {
		return SYMCALL(VA, MSSYM_B1QA9addEffectB1AA5ActorB2AAE26QEAAXAEBVMobEffectInstanceB3AAAA1Z, this, ef);
	}
};

struct Mob : Actor {
	// ��ȡ״̬�б�
	std::vector<MobEffectInstance>* getEffects() {					// IDA Mob::addAdditionalSaveData
		return (std::vector<MobEffectInstance>*)((VA*)this + 152);
	}

	// ��ȡװ������
	VA getArmor() {					// IDA Mob::addAdditionalSaveData
		return VA(this) + 1400;
	}
	// ��ȡ��ͷ����
	VA getHands() {
		return VA(this) + 1408;		// IDA Mob::readAdditionalSaveData
	}
	// ���浱ǰ����������
	VA saveOffhand(VA hlist) {
		return SYMCALL(VA, MSSYM_B1QE11saveOffhandB1AA3MobB2AAA4IEBAB1QA2AVB2QDA6uniqueB1UA3ptrB1AA8VListTagB2AAA1UB2QDA7defaultB1UA6deleteB1AA8VListTagB3AAAA3stdB3AAAA3stdB2AAA2XZ,
			this, hlist);
	}
	// ��ȡ��ͼ��Ϣ
	VA getLevel() {					// IDA Mob::die
		return *((VA*)((VA)this + 816));
	}
};
struct Player : Mob {
#pragma region ͨ������
	// ����API����ȡ�ȼ�װ���б�ֻ��
	static std::string sgetHotbarContainer(Player*);
	// ����API����ȡ���uuid��ֻ��
	static std::string sgetUuid(Player*);
#pragma endregion

	// ȡuuid
	MCUUID* getUuid() {				// IDA ServerNetworkHandler::_createNewPlayer
		return (MCUUID*)((char*)this + 2720);
	}

	// ���ݵ�ͼ��Ϣ��ȡ���xuid
	std::string& getXuid(VA level) {
		return SYMCALL(std::string&, MSSYM_MD5_337bfad553c289ba4656ac43dcb60748,
			level, (char*)this + 2720);
	}

	// ��������������
	void reName(std::string name) {
		SYMCALL(void, MSSYM_B1QA7setNameB1AA6PlayerB2AAA9UEAAXAEBVB2QDA5basicB1UA6stringB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB2AAA1VB2QDA9allocatorB1AA1DB1AA12B2AAA3stdB3AAAA1Z,
			this, name);
	}

	// ��ȡ�����ʶ��
	VA getNetId() {
		return (VA)this + 2432;		// IDA ServerPlayer::setPermissions
	}

	VA getContainerManager() {
		return (VA)this + 2912;		// IDA Player::setContainerManager
	}
	// ��ȡ����
	VA getSupplies() {				// IDA Player::add
		return *(VA*)(*((VA*)this + 366) + 176);
	}
	// ��ȡĩӰ��
	VA getEnderChestContainer() {
		return SYMCALL(VA, MSSYM_B1QE22getEnderChestContainerB1AA6PlayerB2AAE27QEAAPEAVEnderChestContainerB2AAA2XZ, this);
	}
	// ����һ��װ��
	VA setArmor(int i, VA item) {
		return SYMCALL(VA, MSSYM_B1QA8setArmorB1AE12ServerPlayerB2AAE16UEAAXW4ArmorSlotB2AAE13AEBVItemStackB3AAAA1Z, this, i, item);
	}
	// ���ø���
	VA setOffhandSlot(VA item) {
		return SYMCALL(VA, MSSYM_B1QE14setOffhandSlotB1AA6PlayerB2AAE18UEAAXAEBVItemStackB3AAAA1Z, this, item);
	}
	// ���һ����Ʒ
	void addItem(VA item) {
		SYMCALL(VA, MSSYM_B1QA3addB1AA6PlayerB2AAA4UEAAB1UE14NAEAVItemStackB3AAAA1Z, this, item);
	}
	// ��ȡ��ǰѡ�еĿ�λ��
	int getSelectdItemSlot() {			// IDA Player::getSelectedItem
		VA v1 = *((VA*)this + 366);
		return *(unsigned int*)(v1 + 16);
	}
	// ��ȡ��ǰ��Ʒ
	VA getSelectedItem() {
		VA x = SYMCALL(VA, MSSYM_B1QE15getSelectedItemB1AA6PlayerB2AAE17QEBAAEBVItemStackB2AAA2XZ,
			this);
		return x;
	}
	// ��ȡ��Ϸʱ����Ȩ��
	char getPermission() {						// IDA ServerPlayer::setPermissions
		return **(char**)((VA)this + 2112);
	}
	// ������Ϸʱ����Ȩ��
	void setPermission(char m) {
		SYMCALL(void, MSSYM_B1QE14setPermissionsB1AE12ServerPlayerB2AAE29UEAAXW4CommandPermissionLevelB3AAAA1Z,
			this, m);
	}
	// ��ȡ��Ϸʱ����Ȩ��
	char getPermissionLevel() {						// IDA Abilities::setPlayerPermissions
		return (*(char**)((VA)this + 2112))[1];
	}
	// ������Ϸʱ����Ȩ��
	void setPermissionLevel(char m) {
		SYMCALL(void, MSSYM_B1QE20setPlayerPermissionsB1AA9AbilitiesB2AAE28QEAAXW4PlayerPermissionLevelB3AAAA1Z,
			(VA)this + 2112, m);
	}
	// ����������Ʒ�б�
	void updateInventory() {
		VA itm = (VA)this + 4472;				// IDA Player::drop
		SYMCALL(VA, MSSYM_B1QE23forceBalanceTransactionB1AE27InventoryTransactionManagerB2AAA7QEAAXXZ, itm);
	}
	// �������ݰ�
	VA sendPacket(VA pkt) {
		return SYMCALL(VA, MSSYM_B1QE17sendNetworkPacketB1AE12ServerPlayerB2AAE15UEBAXAEAVPacketB3AAAA1Z, this, pkt);
	}
};

#pragma endregion

#pragma region Items ������Ʒ�ṹ��

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
	// ȡ��ƷID
	short getId() {
		return SYMCALL(short,
			MSSYM_B1QA5getIdB1AE13ItemStackBaseB2AAA7QEBAFXZ,
			this);
	}
	// ȡ��Ʒ����ֵ
	short getAuxValue() {
		return SYMCALL(short,
			MSSYM_B1QE11getAuxValueB1AE13ItemStackBaseB2AAA7QEBAFXZ,
			this);
	}
	// ȡ��Ʒ����
	std::string getName() {
		std::string str;
		SYMCALL(__int64,
			MSSYM_MD5_6d581a35d7ad70fd364b60c3ebe93394,
			this, &str);
		return str;
	}
	// ȡ����������
	int getStackSize() {			// IDA ContainerModel::networkUpdateItem
		return *((char*)this + 34);
	}
	// �ж��Ƿ������
	bool isNull() {
		return SYMCALL(bool,
			MSSYM_B1QA6isNullB1AE13ItemStackBaseB2AAA4QEBAB1UA3NXZ,
			this);
	}
};


struct LevelContainerModel {
	// ȡ������
	Player* getPlayer() {
		return ((Player**)this)[26];
	}
};


struct Container {
	VA vtable;
	// ��ȡ������������Ʒ
	VA getSlots(std::vector<ItemStack*>* s) {
		return SYMCALL(VA, MSSYM_B1QA8getSlotsB1AA9ContainerB2AAA4UEBAB1QA2BVB2QDA6vectorB1AE13PEBVItemStackB2AAA1VB2QDA9allocatorB1AE13PEBVItemStackB3AAAA3stdB3AAAA3stdB2AAA2XZ,
			this, s);
	}

};

struct SimpleContainer : Container {
	// ��ȡһ��ָ��������Ʒ
	ItemStack* getItem(int slot) {
		return SYMCALL(ItemStack*, MSSYM_B1QA7getItemB1AE15SimpleContainerB2AAE17UEBAAEBVItemStackB2AAA1HB1AA1Z, this, slot);
	}
	// ����һ��ָ�����ڵ���Ʒ
	VA setItem(int slot, ItemStack* item) {
		return SYMCALL(VA, MSSYM_B1QA7setItemB1AE15SimpleContainerB2AAE19UEAAXHAEBVItemStackB3AAAA1Z,
			this, slot, item);
	}
};

struct FillingContainer : Container {
	// ��ʽ������������Ʒ��tag
	VA save(VA tag) {
		return SYMCALL(VA, MSSYM_B1QA4saveB1AE16FillingContainerB2AAA4QEAAB1QA2AVB2QDA6uniqueB1UA3ptrB1AA8VListTagB2AAA1UB2QDA7defaultB1UA6deleteB1AA8VListTagB3AAAA3stdB3AAAA3stdB2AAA2XZ,
			this, tag);
	}
	// ����������ָ��λ�õ���Ʒ
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
	// ȡ������
	Player* getPlayer() {				// IDA ContainerManagerModel::ContainerManagerModel
		return *reinterpret_cast<Player**>(reinterpret_cast<VA>(this) + 8);
	}
};
struct LevelContainerManagerModel
	:ContainerManagerModel {
};

#pragma endregion

#pragma region Packets �������ݰ��ṹ��

struct TextPacket {
	char filler[0xC8];
	// ȡ�����ı�
	std::string toString() {			// IDA ServerNetworkHandler::handle
		std::string str = std::string(*(std::string*)((VA)this + 80));
		return str;
	}
};

struct CommandRequestPacket {
	char filler[0x90];
	// ȡ�����ı�
	std::string toString() {			// IDA ServerNetworkHandler::handle
		std::string str = std::string(*(std::string*)((VA)this + 40));
		return str;
	}
};

struct ModalFormRequestPacket {
	char filler[0x48];
};

struct ModalFormResponsePacket {
	// ȡ�����ID
	UINT getFormId() {
		return *(UINT*)((VA)this + 40);
	}
	// ȡѡ�����
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