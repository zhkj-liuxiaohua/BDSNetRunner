#pragma once
#include <string>
#include "THook.h"
#include "RVAs.hpp"
#include <json/json.h>
#include "Component.h"

// ��������ṹ��
#pragma region Blocks ���巽��ṹ��

struct BPos3 {
	INT32 x;
	INT32 y;
	INT32 z;
public:
	std::string toJsonString() {
		char str[256];
		sprintf_s(str, "{\"x\":%d,\"y\":%d,\"z\":%d}", x, y, z);
		return std::string(str);
	}
};

struct BlockLegacy {
	// ��ȡ������
	auto getFullName() const {				// IDA BlockLegacy::~BlockLegacy
		return (std::string&) * (__int64*)((__int64)this + 120);
	}
	// ��ȡ����ID��
	auto getBlockItemID() const {			// IDA BlockLegacy::BlockLegacy VanillaItems::serverInitCreativeItemsCallback Item::beginCreativeGroup "itemGroup.name.planks"
		short v3 = *(short*)((VA)this + 312);
		if (v3 < 0x100) {
			return v3;
		}
		return (short)(255 - v3);
	}
};

struct BlockPos : BPos3 {
	// ��ȡ��������ͷ
	BPos3* getPosition() const {
		return (BPos3*)this;
	}
	//ͨ�� BDS ��ָ��ԭ�����
	std::string toString() {
		std::string s;
		SYMCALL(std::string&, MSSYM_MD5_08038beb99b82fbb46756aa99d94b86f, this, &s);
		return s;
	}
	// ͨ�� Vec3 ���� BlockPos
	BlockPos(const void* vec3) {
		auto v = (float*)vec3;
		x = v ? (int)v[0] : 0;
		y = v ? (int)v[1] : 0;
		z = v ? (int)v[2] : 0;
	}
	// ͨ�� double ���� BlockPos
	BlockPos(double x2, double y2, double z2) {
		x = (int)x2;
		y = (int)y2;
		z = (int)z2;
	}
	BlockPos() {
		memset(this, 0, sizeof(BlockPos));
	}
};

struct Block {
	// ��ȡԴ
	const BlockLegacy* getLegacyBlock() const {			// IDA LegacyStructureTemplate::_mapToProperty "waterlogged"
		return SYMCALL(BlockLegacy*, MSSYM_B1QE14getLegacyBlockB1AA5BlockB2AAE19QEBAAEBVBlockLegacyB2AAA2XZ, this);
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
		return *(int*)(*((VA*)this + 4) + 200);
	}
	// ��ȡָ����Χ������ʵ��
	std::vector<VA*>* getEntities(VA *rect) {
		return SYMCALL(std::vector<VA*>*, MSSYM_MD5_73d55bcf0da8c45a15024daf84014ad7,
			this, 0, rect, 1);
	}
	// ��ȡָ����Χ���������
	std::vector<VA*>* getPlayers(VA* rect) {
		return SYMCALL(std::vector<VA*>*, MSSYM_MD5_73d55bcf0da8c45a15024daf84014ad7,
			this, ActorType::Player_0, rect, 0);
	}
	//��ȡ���������
	BlockActor* getBlockActor(const BlockPos* blkpos) {
		return SYMCALL(BlockActor*, MSSYM_B1QE14getBlockEntityB1AE11BlockSourceB2AAE18QEAAPEAVBlockActorB2AAE12AEBVBlockPosB3AAAA1Z,
			this, blkpos);
	}
};

struct Dimension {
	// ��ȡ����Դ
	VA getBlockSource() {					// IDA Level::tickEntities
		return *((VA*)this + 10);
	}
};

struct Level {
	// ��ȡά��
	Dimension* getDimension(int did) {
		return SYMCALL(Dimension*, MSSYM_B1QE12getDimensionB1AA5LevelB2AAE17QEBAPEAVDimensionB2AAA1VB2QDE11AutomaticIDB1AE10VDimensionB2AAA1HB3AAAA1Z,
			this, did);
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
	// ����API��ģ���ʵ�����һ��Դ����һʵ����˺�
	static bool shurt(Actor*, Actor*, ActorDamageCause, int, bool, bool);
	// ����API�����ݲ�ѯID����ʵ��ָ��
	static Actor* sgetfromUniqueID(VA);
	// ����API����ѯָ��ά��ָ�����귶Χ������ʵ��
	static std::vector<VA*>* sgetEntities(int, float, float, float, float, float, float);
#pragma endregion

	// ȡ����Դ
	BlockSource* getRegion() {						// IDA
		return (BlockSource*)*((VA*)this + 105);
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
		return *reinterpret_cast<BYTE*>(reinterpret_cast<VA>(this) + 448);
	}

	// ��ȡ���ﵱǰ��������
	Vec3* getPos() {
		return SYMCALL(Vec3*,
			MSSYM_B1QA6getPosB1AA5ActorB2AAE12UEBAAEBVVec3B2AAA2XZ, this);
	}

#if 0
	// ��ȡ�������ͣ��޷���ȡ����������ȷ���ͣ�����������
	std::string getTypeName() {
		std::string actor_typename;					// IDA ActorCommandOrigin::getName CommandUtils::getActorName
		SYMCALL(std::string&,
			MSSYM_MD5_41a18e1578312643b066a562efefb36a,
			&actor_typename, this);
		return actor_typename;
	}
#endif

	// ��ȡʵ������
	int getEntityTypeId() {
		return sgetEntityTypeId(this);
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

#if 0			// TODO MODULE_KENGWANG, XXX
	// ���
	void AddRider(Actor* actor) {
		SYMCALL(void,
			MSSYM_B1QA8addRiderB1AA5ActorB2AAE10UEAAXAEAV1B2AAA1Z,
			this, actor);
	}
#endif
};

struct Mob : Actor {
	// ��ȡװ������
	VA getArmor() {					// IDA Mob::addAdditionalSaveData
		return VA(this) + 1472;
	}
	// ��ȡ��ͷ����
	VA getHands() {
		return VA(this) + 1480;		// IDA Mob::readAdditionalSaveData
	}
	// ��ȡ��ͼ��Ϣ
	VA getLevel() {					// IDA Mob::die
		return *((VA*)((VA)this + 856));
	}
};
struct Player : Mob {
#pragma region ͨ������
	// ����API����ȡ�ȼ�װ���б�ֻ��
	static std::string sgetHotbarContainer(Player*);
	// ����API����ȡ���uuid��ֻ��
	static std::string sgetUuid(Player*);
	// ����API����ȡ�������IP�Ͷ˿ڣ�ֻ��
	static std::string sgetIPPort(Player*);
	// ����API��������ҵȼ�
	static void saddLevel(Player*, int);
	// ����API����ȡ��Ҷ�Ӧ�Ʒְ�IDֵ
	static __int64 sgetScoreboardId(Player*);
	// ����API��������Ҷ�Ӧ�Ʒְ岢��ȡ��IDֵ
	static __int64 screateScoreboardId(Player*);
	// ����API����ѯָ��ά��ָ�����귶Χ���������
	static std::vector<VA*>* sgetPlayers(int, float, float, float, float, float, float);
#pragma endregion

	// ȡuuid
	MCUUID* getUuid() {				// IDA ServerNetworkHandler::_createNewPlayer
		return (MCUUID*)((char*)this + 2800);
	}

	// ���ݵ�ͼ��Ϣ��ȡ���xuid
	std::string& getXuid(VA level) {
		return SYMCALL(std::string&, MSSYM_MD5_337bfad553c289ba4656ac43dcb60748,
			level, (char*)this + 2800);
	}

	// ��������������
	void reName(std::string name) {
		SYMCALL(void, MSSYM_B1QA7setNameB1AA6PlayerB2AAA9UEAAXAEBVB2QDA5basicB1UA6stringB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB2AAA1VB2QDA9allocatorB1AA1DB1AA12B2AAA3stdB3AAAA1Z,
			this, name);
	}

	// ��ȡ�����ʶ��
	VA getNetId() {
		return (VA)this + 2512;		// IDA ServerPlayer::setPermissions
	}
	// ��ȡ����
	VA getSupplies() {				// IDA Player::add
		return *(VA*)(*((VA*)this + 378) + 176);
	}

	// ���һ����Ʒ
	void addItem(VA item) {
		SYMCALL(VA, MSSYM_B1QA3addB1AA6PlayerB2AAA4UEAAB1UE14NAEAVItemStackB3AAAA1Z, this, item);
	}

	// ����������Ʒ�б�
	void updateInventory() {
		VA itm = (VA)this + 4568;				// IDA Player::drop
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
	// ת��idΪԭʼ��ʶ�ַ�
	static std::string getRawNameFromId(int id) {
		VA* it;
		SYMCALL(VA, MSSYM_B1QA7getItemB1AE12ItemRegistryB2AAA2SAB1QA2AVB2QDA7WeakPtrB1AA5VItemB4AAAAA1FB1AA1Z, &it, id);
		if (it && *it) {
			return *SYMCALL(std::string*, MSSYM_MD5_2f9d68ca736b0da0c26f063f568898bc,	// Item::getRawNameId
				*it);
		}
		return "unknow";
	}
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
		std::string str;				// IDA ItemStackBase::getName
		SYMCALL(__int64,
			MSSYM_MD5_6d581a35d7ad70fd364b60c3ebe93394,
			this, &str);
		return str;
	}
	// ȡ��Ʒ�����ռ��ʶ
	std::string getRawNameId() {
		std::string str;
		SYMCALL(VA, MSSYM_MD5_2f9d68ca736b0da0c26f063f568898bc,
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

struct ItemActor : Actor {
	// ��ȡʵ����Ʒ
	ItemStack* getItemStack() {		// IDA   see Hopper::_addItem
		return (ItemStack*)((VA)this + 1648);
	}
};

struct LevelContainerModel {
	// ȡ������
	Player* getPlayer() {
		return ((Player**)this)[26];
	}
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