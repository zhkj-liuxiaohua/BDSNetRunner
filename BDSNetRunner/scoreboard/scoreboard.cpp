#include "scoreboard.hpp"
#include "../mod.h"

#if (MODULE_05007)

Scoreboard* scoreboard;//储存计分板名称

//获取玩家在指定计分板下的内容（返回整数，在这个计分板下这个玩家的值）
int getscoreboard(Player* p, std::string objname) {
	auto testobj = scoreboard->getObjective(objname);
	if (!testobj) {
		std::cout << u8"[CSR] 未能找到对应计分板，自动创建：" << objname << std::endl;
		testobj = scoreboard->addObjective(objname, objname);
		return 0;
	}
	__int64 a2[2];
	auto scoreid = scoreboard->getScoreboardID(p);
	auto scores = ((Objective*)testobj)->getplayerscoreinfo((ScoreInfo*)a2, scoreid);
	return scores->getcount();
}

bool setscoreboard(Player* p, std::string objname, int count) {
	auto testobj = scoreboard->getObjective(objname);
	if (!testobj) {
		std::cout << u8"[CSR] 未能找到对应计分板，自动创建: " << objname << std::endl;
		testobj = scoreboard->addObjective(objname, objname);
	}
	if (!testobj)
		return false;
	VA scoreid = (VA)scoreboard->getScoreboardID(p);
	if (scoreid == (VA)SYM_POINT(VA, MSSYM_B1QA7INVALIDB1AE12ScoreboardIdB2AAA32U1B1AA1A)) {
		std::cout << u8"[CSR] 未能找到玩家对应计分板，自动设置: " << objname << std::endl;
		scoreid = scoreboard->createPlayerScoreboardId(p);
	}
	if (!scoreid || scoreid == (VA)SYM_POINT(VA, MSSYM_B1QA7INVALIDB1AE12ScoreboardIdB2AAA32U1B1AA1A))
		return false;
	scoreboard->modifyPlayerScore((ScoreboardId*)scoreid, (Objective*)testobj, count);
	return true;
}

// 计分板命令注册（开服时获取所有的计分板名称）
THook(void*, MSSYM_B2QQE170ServerScoreboardB2AAA4QEAAB1AE24VCommandSoftEnumRegistryB2AAE16PEAVLevelStorageB3AAAA1Z, void* _this, void* a2, void* a3) {
	scoreboard = (Scoreboard*)original(_this, a2, a3);
	return scoreboard;
}

#endif
