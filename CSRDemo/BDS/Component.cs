using System;
using System.Collections;
using System.Runtime.InteropServices;

namespace CSR
{
    public enum ActorDamageCause : int
    {
        Override = 0x0,
        Contact = 0x1,
        EntityAttack = 0x2,
        Projectile = 0x3,
        Suffocation = 0x4,
        Fall = 0x5,
        Fire = 0x6,
        FireTick = 0x7,
        Lava = 0x8,
        Drowning = 0x9,
        BlockExplosion = 0x0A,
        EntityExplosion = 0x0B,
        Void = 0x0C,
        Suicide = 0x0D,
        Magic = 0x0E,
        Wither = 0x0F,
        Starve = 0x10,
        Anvil = 0x11,
        Thorns = 0x12,
        FallingBlock = 0x13,
        Piston = 0x14,
        FlyIntoWall = 0x15,
        Magma = 0x16,
        Fireworks = 0x17,
        Lightning = 0x18,
        Charging = 0x19,
        Temperature = 0x1A,
        All = 0x1F,
        None = -0x01
    }

    /// <summary>
    /// 实体类
    /// </summary>
    public class CsActor
    {
        const string ENTITY_GET_ARMOR_CONTAINER = "entity.get_armor_container";
        const string ENTITY_GET_ATTACK = "entity.get_attack";
        const string ENTITY_SET_ATTACK = "entity.set_attack";
        const string ENTITY_GET_COLLISION_BOX = "entity.get_collision_box";
        const string ENTITY_SET_COLLISION_BOX = "entity.set_collision_box";
        const string ENTITY_GET_HAND_CONTAINER = "entity.get_hand_container";
        const string ENTITY_GET_HEALTH = "entity.get_health";
        const string ENTITY_SET_HEALTH = "entity.set_health";
        const string ENTITY_GET_INVENTORY_CONTAINER = "entity.get_inventory_container";
        const string ENTITY_GET_NAME = "entity.get_name";
        const string ENTITY_SET_NAME = "entity.set_name";
        const string ENTITY_GET_POSITION = "entity.get_position";
        const string ENTITY_SET_POSITION = "entity.set_position";
        const string ENTITY_GET_ROTATION = "entity.get_rotation";
        const string ENTITY_SET_ROTATION = "entity.set_rotation";
        const string ENTITY_GET_DIMENSIONID = "entity.get_dimensionid";
        const string ENTITY_GET_TYPEID = "entity.get_typeid";
        const string ENTITY_GET_UNIQUEID = "entity.get_uniqueid";
        const string ENTITY_REMOVE = "entity.remove";
        const string ENTITY_HURT = "entity.hurt";
        #region 非社区内容
        const string ENTITY_GET_ABILITIES = "entity.get_abilities";
        const string ENTITY_SET_ABILITIES = "entity.set_abilities";
        const string ENTITY_GET_ATTRIBUTES = "entity.get_attributes";
        const string ENTITY_SET_ATTRIBUTES = "entity.set_attributes";
        const string ENTITY_GET_MAXATTRIBUTES = "entity.get_maxattributes";
        const string ENTITY_SET_MAXATTRIBUTES = "entity.set_maxattributes";
        const string ENTITY_GET_EFFECTS = "entity.get_effects";
        const string ENTITY_SET_EFFECTS = "entity.set_effects";
        #endregion
        const string LEVEL_GETFROM_UNIQUEID = "level.getfrom_uniqueid";
        const string LEVEL_GETSFROM_AABB = "level.getsfrom_aabb";

        protected delegate Std_String AGET(IntPtr p);
        protected delegate int AGETDIMENSIONID(IntPtr p);
        protected delegate bool ASET(IntPtr p, string s);
        protected delegate bool ASETNAME(IntPtr p, string n, bool a);
        protected delegate ulong AGETUNIQUEID(IntPtr p);
        protected delegate bool AREMOVE(IntPtr p);
        protected delegate bool AHURT(IntPtr p, IntPtr sp, ActorDamageCause cause, int count, bool knock, bool ignite);
        protected delegate IntPtr AGETFROMUNIQUEID(ulong i);
        protected delegate IntPtr AGETSFROMAABB(int did, float x1, float y1, float z1, float x2, float y2, float z2);
        static protected AGET egetArmorContainer;
        static protected AGET egetAttack;
        static protected ASET esetAttack;
        static protected AGET egetCollisionBox;
        static protected ASET esetCollisionBox;
        static protected AGET egetHandContainer;
        static protected AGET egetHealth;
        static protected ASET esetHealth;
        static protected AGET egetInventoryContainer;
        static protected AGET egetName;
        static protected ASETNAME esetName;
        static protected AGET egetPosition;
        static protected ASET esetPosition;
        static protected AGET egetRotation;
        static protected ASET esetRotation;
        static protected AGETDIMENSIONID egetDimensionId;
        static protected AGETDIMENSIONID egetTypeId;
        static protected AGETUNIQUEID egetUniqueId;
        static protected AREMOVE eremove;
        static protected AHURT ehurt;
        #region 非社区内容
        static protected AGET egetAbilities;
        static protected ASET esetAbilities;
        static protected AGET egetAttributes;
        static protected ASET esetAttributes;
        static protected AGET egetMaxAttributes;
        static protected ASET esetMaxAttributes;
        static protected AGET egetEffects;
        static protected ASET esetEffects;
        #endregion
        static protected AGETFROMUNIQUEID egetFromUniqueId;
        static protected AGETSFROMAABB egetsFromAABB;
        static bool entityApiInited = false;

        static protected bool initEntityAPI(MCCSAPI api)
        {
            if (!entityApiInited)
            {
                if (api != null)
                {
                    egetArmorContainer = api.ConvertComponentFunc<AGET>(ENTITY_GET_ARMOR_CONTAINER);
                    egetAttack = api.ConvertComponentFunc<AGET>(ENTITY_GET_ATTACK);
                    esetAttack = api.ConvertComponentFunc<ASET>(ENTITY_SET_ATTACK);
                    egetCollisionBox = api.ConvertComponentFunc<AGET>(ENTITY_GET_COLLISION_BOX);
                    esetCollisionBox = api.ConvertComponentFunc<ASET>(ENTITY_SET_COLLISION_BOX);
                    egetHandContainer = api.ConvertComponentFunc<AGET>(ENTITY_GET_HAND_CONTAINER);
                    egetHealth = api.ConvertComponentFunc<AGET>(ENTITY_GET_HEALTH);
                    esetHealth = api.ConvertComponentFunc<ASET>(ENTITY_SET_HEALTH);
                    egetInventoryContainer = api.ConvertComponentFunc<AGET>(ENTITY_GET_INVENTORY_CONTAINER);
                    egetName = api.ConvertComponentFunc<AGET>(ENTITY_GET_NAME);
                    esetName = api.ConvertComponentFunc<ASETNAME>(ENTITY_SET_NAME);
                    egetPosition = api.ConvertComponentFunc<AGET>(ENTITY_GET_POSITION);
                    esetPosition = api.ConvertComponentFunc<ASET>(ENTITY_SET_POSITION);
                    egetRotation = api.ConvertComponentFunc<AGET>(ENTITY_GET_ROTATION);
                    esetRotation = api.ConvertComponentFunc<ASET>(ENTITY_SET_ROTATION);
                    egetDimensionId = api.ConvertComponentFunc<AGETDIMENSIONID>(ENTITY_GET_DIMENSIONID);
                    egetTypeId = api.ConvertComponentFunc<AGETDIMENSIONID>(ENTITY_GET_TYPEID);
                    egetUniqueId = api.ConvertComponentFunc<AGETUNIQUEID>(ENTITY_GET_UNIQUEID);
                    eremove = api.ConvertComponentFunc<AREMOVE>(ENTITY_REMOVE);
                    ehurt = api.ConvertComponentFunc<AHURT>(ENTITY_HURT);
                    if (api.COMMERCIAL)
                    {   // 非社区内容
                        egetAbilities = api.ConvertComponentFunc<AGET>(ENTITY_GET_ABILITIES);
                        esetAbilities = api.ConvertComponentFunc<ASET>(ENTITY_SET_ABILITIES);
                        egetAttributes = api.ConvertComponentFunc<AGET>(ENTITY_GET_ATTRIBUTES);
                        esetAttributes = api.ConvertComponentFunc<ASET>(ENTITY_SET_ATTRIBUTES);
                        egetMaxAttributes = api.ConvertComponentFunc<AGET>(ENTITY_GET_MAXATTRIBUTES);
                        esetMaxAttributes = api.ConvertComponentFunc<ASET>(ENTITY_SET_MAXATTRIBUTES);
                        egetEffects = api.ConvertComponentFunc<AGET>(ENTITY_GET_EFFECTS);
                        esetEffects = api.ConvertComponentFunc<ASET>(ENTITY_SET_EFFECTS);
                    }
                    egetFromUniqueId = api.ConvertComponentFunc<AGETFROMUNIQUEID>(LEVEL_GETFROM_UNIQUEID);
                    egetsFromAABB = api.ConvertComponentFunc<AGETSFROMAABB>(LEVEL_GETSFROM_AABB);
                    entityApiInited = true;
                } else
                {
                    return false;
                }
            }
            return true;
        }

        static protected MCCSAPI mapi;
        protected IntPtr ptr;

        public CsActor(MCCSAPI api, IntPtr e)
        {
            if (mapi == null)
                mapi = api;
            ptr = e;
            initEntityAPI(api);
        }

        /// <summary>
        /// 装备栏
        /// </summary>
        public string ArmorContainer { get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = egetArmorContainer(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            } }
        /// <summary>
        /// 攻击力
        /// </summary>
        public string Attack { get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = egetAttack(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            } set {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    esetAttack(ptr, value);
                }
            }
        }
        /// <summary>
        /// 碰撞箱
        /// </summary>
        public string CollisionBox { get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = egetCollisionBox(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            } set
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    esetCollisionBox(ptr, value);
                }
            }
        }
        /// <summary>
        /// 主副手栏
        /// </summary>
        public string HandContainer
        {
            get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = egetHandContainer(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            }
        }
        /// <summary>
        /// 生命值
        /// </summary>
        public string Health { get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = egetHealth(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            }
            set
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    esetHealth(ptr, value);
                }
            }
        }
        /// <summary>
        /// 背包列表
        /// </summary>
        public string InventoryContainer
        {
            get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = egetInventoryContainer(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            }
        }
        /// <summary>
        /// 获取名字
        /// </summary>
        /// <returns></returns>
        public string getName()
        {
            if (ptr != null && ptr != IntPtr.Zero)
            {
                Std_String s = egetName(ptr);
                return StrTool.c_str(s);
            }
            return null;
        }
        /// <summary>
        /// 设置名字及显示属性
        /// </summary>
        /// <param name="nname">名字</param>
        /// <param name="alwaysShow">是否常显示</param>
        /// <returns></returns>
        public bool setName(string nname, bool alwaysShow)
        {
            if (ptr != null && ptr != IntPtr.Zero)
            {
                return esetName(ptr, nname, alwaysShow);
            }
            return false;
        }
        /// <summary>
        /// 实体所在坐标
        /// </summary>
        public string Position
        {
            get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = egetPosition(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            }
            set
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    esetPosition(ptr, value);
                }
            }
        }
        /// <summary>
        /// 实体转角属性
        /// </summary>
        public string Rotation
        {
            get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = egetRotation(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            }
            set
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    esetRotation(ptr, value);
                }
            }
        }
        /// <summary>
        /// 维度ID
        /// </summary>
        public int DimensionId { get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    return egetDimensionId(ptr);
                }
                return -1;
            } }
        /// <summary>
        /// 实体类型ID
        /// </summary>
        public int TypeId
        {
            get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    return egetTypeId(ptr);
                }
                return 1;
            }
        }
        /// <summary>
        /// 查询ID
        /// </summary>
        public ulong UniqueId { get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    return egetUniqueId(ptr);
                }
                return 0;
            } }
        /// <summary>
        /// 从地图中移除该实体
        /// </summary>
        /// <returns>是否移除成功</returns>
        public bool remove()
        {
            if (ptr != null && ptr != IntPtr.Zero)
            {
                bool ret = eremove(ptr);
                if (ret)
                {
                    ptr = IntPtr.Zero;
                    return ret;
                }
            }
            return false;
        }
        /// <summary>
        /// 模拟产生一个由源实体发出的伤害
        /// </summary>
        /// <param name="sourceActor">源实体</param>
        /// <param name="cause">伤害类型</param>
        /// <param name="count">具体伤害数值</param>
        /// <param name="knock">是否产生击退</param>
        /// <param name="ignite">是否产生火焰附加</param>
        /// <returns>是否伤害成功</returns>
        public bool hurt(IntPtr sourceActor, ActorDamageCause cause, int count, bool knock, bool ignite)
        {
            if (ptr != null && ptr != IntPtr.Zero)
            {
                return ehurt != null && ehurt(ptr, sourceActor, cause, count, knock, ignite);
            }
            return false;
        }

        #region 非社区内容
        /// <summary>
        /// 实体能力值列表
        /// </summary>
        public string Abilities
        {
            get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    if (egetAbilities != null)
                    {
                        Std_String str = egetAbilities(ptr);
                        return StrTool.c_str(str);
                    }
                }
                return "";
            }
            set
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    if (esetAbilities != null)
                        esetAbilities(ptr, value);
                }
            }
        }
        /// <summary>
        /// 实体属性列表
        /// </summary>
        public string Attributes
        {
            get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    if (egetAttributes != null)
                    {
                        Std_String str = egetAttributes(ptr);
                        return StrTool.c_str(str);
                    }
                }
                return "";
            }
            set
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    if (esetAttributes != null)
                        esetAttributes(ptr, value);
                }
            }
        }
        /// <summary>
        /// 实体属性最大值列表
        /// </summary>
        public string MaxAttributes
        {
            get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    if (egetMaxAttributes != null)
                    {
                        Std_String str = egetMaxAttributes(ptr);
                        return StrTool.c_str(str);
                    }
                }
                return "";
            }
            set
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    if (esetMaxAttributes != null)
                        esetMaxAttributes(ptr, value);
                }
            }
        }
        /// <summary>
        /// 实体所有效果状态列表
        /// </summary>
        public string Effects
        {
            get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    if (egetEffects != null)
                    {
                        Std_String str = egetEffects(ptr);
                        return StrTool.c_str(str);
                    }
                }
                return "";
            }
            set
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    if (esetEffects != null)
                        esetEffects(ptr, value);
                }
            }
        }
        #endregion

        /// <summary>
        /// 从查询ID处反查一个实体（或玩家）
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        public static CsActor getFromUniqueId(MCCSAPI api, ulong id)
        {
            if (egetFromUniqueId == null)
                initEntityAPI(api);
            IntPtr p = egetFromUniqueId(id);
            if (p != null && p != IntPtr.Zero)
            {
                if (egetTypeId != null && egetTypeId(p) == 319)
                    return new CsPlayer(api, p);
                else
                    return new CsActor(api, p);
            }
            return null;
        }
        /// <summary>
        /// 从指定地图位置查询实体指针列表(不含玩家)
        /// </summary>
        /// <param name="api"></param>
        /// <param name="did">维度ID</param>
        /// <param name="x1"></param>
        /// <param name="y1"></param>
        /// <param name="z1"></param>
        /// <param name="x2"></param>
        /// <param name="y2"></param>
        /// <param name="z2"></param>
        /// <returns>实体指针列表</returns>
        public static ArrayList getsFromAABB(MCCSAPI api, int did, float x1, float y1, float z1, float x2, float y2, float z2) {
            if (egetsFromAABB == null)
                initEntityAPI(api);
            IntPtr pv = egetsFromAABB(did, x1, y1, z1, x2, y2, z2);
            if (pv != null && pv != IntPtr.Zero) {
                try {
                    return ((Std_Vector)Marshal.PtrToStructure(pv, typeof(Std_Vector))).toList();
                }
                catch { }
            }
            return null;
        }
    }

    /// <summary>
    /// 玩家类
    /// </summary>
    public class CsPlayer : CsActor
    {
        const string PLAYER_GET_HOTBAR_CONTAINER = "player.get_hotbar_container";
        const string PLAYER_GET_UUID = "player.get_uuid";
        const string PLAYER_GET_IPPORT = "player.get_ipport";
        const string PLAYER_ADD_LEVEL = "player.add_level";
        const string PLAYER_GET_SCOREID = "player.get_scoreboardid";
        const string PLAYER_CREATE_SCOREID = "player.create_scoreboardid";
        const string LEVEL_GETPLFROM_AABB = "level.getplfrom_aabb";
        #region 非社区内容
        const string PLAYER_GET_STORAGEID = "player.get_storageid";
        #endregion

        protected delegate bool PADDLEVEL(IntPtr p, int lv);
        protected delegate long PGETSCOREID(IntPtr p);
        static AGET pgetHotbarContainer, pgetServerStorageID;
        static AGET pgetUuid;
        static AGET pgetIPPort;
        static PADDLEVEL paddLevel;
        static AGETSFROMAABB pgetplFromAABB;
        static PGETSCOREID pgetScoreboardId, pcreateScoreboardId;
        static bool playerApiInited = false;

        static private bool initPlayerAPI(MCCSAPI api)
        {
            if (!playerApiInited)
            {
                if (api != null)
                {
                    pgetHotbarContainer = api.ConvertComponentFunc<AGET>(PLAYER_GET_HOTBAR_CONTAINER);
                    pgetUuid = api.ConvertComponentFunc<AGET>(PLAYER_GET_UUID);
                    pgetIPPort = api.ConvertComponentFunc<AGET>(PLAYER_GET_IPPORT);
                    paddLevel = api.ConvertComponentFunc<PADDLEVEL>(PLAYER_ADD_LEVEL);
                    pgetScoreboardId = api.ConvertComponentFunc<PGETSCOREID>(PLAYER_GET_SCOREID);
                    pcreateScoreboardId = api.ConvertComponentFunc<PGETSCOREID>(PLAYER_CREATE_SCOREID);
                    pgetplFromAABB = api.ConvertComponentFunc<AGETSFROMAABB>(LEVEL_GETPLFROM_AABB);
                    if (api.COMMERCIAL)
                    {   // 非社区内容
                        pgetServerStorageID = api.ConvertComponentFunc<AGET>(PLAYER_GET_STORAGEID);
                    }
                    playerApiInited = true;
                }
                else
                {
                    return false;
                }
            }
            return true;
        }

        public CsPlayer(MCCSAPI api, IntPtr p) : base(api, p)
        {
            initPlayerAPI(api);
        }
        /// <summary>
        /// 获取玩家热键栏
        /// </summary>
        public string HotbarContainer { get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = pgetHotbarContainer(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            } }
        /// <summary>
        /// 获取玩家uuid
        /// </summary>
        public string Uuid { get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = pgetUuid(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            } }
        /// <summary>
        /// 获取玩家IP和端口
        /// </summary>
        public string IpPort { get
            {
                if (ptr != null && ptr != IntPtr.Zero)
                {
                    Std_String s = pgetIPPort(ptr);
                    return StrTool.c_str(s);
                }
                return null;
            } }
        /// <summary>
        /// 增加玩家等级
        /// </summary>
        /// <param name="lv"></param>
        public void addLevel(int lv)
        {
            if (ptr != null && ptr != IntPtr.Zero)
            {
                paddLevel(ptr, lv);
            }
        }
        /// <summary>
        /// 获取玩家对应计分板ID数值
        /// </summary>
        /// <returns>计分板ID值</returns>
        public long getScoreboardId()
        {
            return (ptr != null && ptr != IntPtr.Zero) ?
                pgetScoreboardId(ptr) : -1;
        }
        /// <summary>
        /// 创建玩家对应计分板ID并获取其值
        /// </summary>
        /// <returns>计分板ID值</returns>
        public long createScoreboardId()
        {
            return (ptr != null && ptr != IntPtr.Zero) ?
                pcreateScoreboardId(ptr) : -1;
        }
        #region 非社区内容
        public string StorageID
        {
            get
            {
                return (ptr != null && ptr != IntPtr.Zero) ?
                    StrTool.c_str(pgetServerStorageID(ptr)) : string.Empty;
            }
        }
        #endregion
        /// <summary>
        /// 从指定地图位置查询玩家指针列表
        /// </summary>
        /// <param name="api"></param>
        /// <param name="did">维度ID</param>
        /// <param name="x1"></param>
        /// <param name="y1"></param>
        /// <param name="z1"></param>
        /// <param name="x2"></param>
        /// <param name="y2"></param>
        /// <param name="z2"></param>
        /// <returns>玩家指针列表</returns>
        public static ArrayList getplFromAABB(MCCSAPI api, int did, float x1, float y1, float z1, float x2, float y2, float z2)
        {
            if (pgetplFromAABB == null)
                initEntityAPI(api);
            IntPtr pv = pgetplFromAABB(did, x1, y1, z1, x2, y2, z2);
            if (pv != null && pv != IntPtr.Zero)
            {
                try
                {
                    return ((Std_Vector)Marshal.PtrToStructure(pv, typeof(Std_Vector))).toList();
                }
                catch { }
            }
            return null;
        }
    }
}
