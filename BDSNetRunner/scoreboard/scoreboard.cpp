#include "scoreboard.hpp"
#include "../mod.h"

#if (MODULE_05007)

static Scoreboard* scoreboard;//储存计分板名称

//获取玩家在指定计分板下的内容（返回整数，在这个计分板下这个玩家的值）
int getscoreboard(Player* p,std::string objname) {
	auto testobj = scoreboard->getobject(&objname);
	if (!testobj) {
		std::cout << u8"没有找到对应计分板，自动创建: " << objname << std::endl;
		std::string cmd = "scoreboard objectives add \"" + objname + "\" dummy money";
		runcmd(cmd.c_str());
		return 0;
	}

	__int64 a2[2];
	auto scoreid = scoreboard->getScoreboardID(p);
	auto scores = testobj->getplayerscoreinfo((ScoreInfo*)a2, scoreid);

	return (__int64)scores->getcount();
}

// 计分板命令注册（开服时获取所有的计分板名称）
THook(void*, MSSYM_B2QQE170ServerScoreboardB2AAA4QEAAB1AE24VCommandSoftEnumRegistryB2AAE16PEAVLevelStorageB3AAAA1Z, void* _this, void* a2, void* a3) {
	scoreboard = (Scoreboard*)original(_this, a2, a3);
	return scoreboard;
}

#endif
