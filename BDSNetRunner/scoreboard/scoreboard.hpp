#pragma once
#include "../mod.h"

#if (MODULE_05007)

extern int getscoreboard(Player*, std::string);

//结构体===============================================================================================================

//摘自计分板转bdxmoney项目：https://github.com/kkj9333/BDS--
//原作者：starkc

//函数===========================================================================
//获取计分板内容

struct ScoreboardId {
	//
};
struct PlayerScoreboardId {
	//
};
struct PlayerScore {
	//  *(_QWORD *)this = *(_QWORD *)a2;//ScoreboardId *a2
	//*((_QWORD*)this + 1) = *((_QWORD*)a2 + 1);
	// *((_DWORD *)this + 4) = int;
	auto getscore() {
		return *((__int64*)this + 4);
	}
};
struct ScoreInfo {
	//scoreboardcmd list; objective::objective; scoreboard getscores
	//int scores    +12 this[12]
	// std::string displayname  +96
	//std::string name +64
	auto getcount() {
		return *(int*)((VA)(this) + 12);
	}
};
struct Objective {
	auto getscoreinfo(ScoreInfo* a1, ScoreboardId* a2) {
		return SYMCALL(ScoreInfo*, MSSYM_B1QE14getPlayerScoreB1AA9ObjectiveB2AAA4QEBAB1QE11AUScoreInfoB2AAE16AEBUScoreboardIdB3AAAA1Z, this, a1, a2);
	}
	auto getplayerscoreinfo(ScoreInfo* a1, PlayerScoreboardId* a2) {
		return SYMCALL(ScoreInfo*, MSSYM_B1QE14getPlayerScoreB1AA9ObjectiveB2AAA4QEBAB1QE11AUScoreInfoB2AAE16AEBUScoreboardIdB3AAAA1Z, this, a1, a2);
	}
};

struct IdentityDictionary {
	//4个unmap
	auto getScoreboardID(PlayerScoreboardId* a2) {
		return SYMCALL(ScoreboardId*, MSSYM_MD5_6f6eac4360ca6db559c6b6e16774f7fa, this, a2, this);
	}
};
struct ScoreboardIdentityRef {
	//bool请设置为1; a6=0 set,a6=1,add,a6=2,remove
	bool modifiedscores(Objective* obj, __int64 num, unsigned __int8 setfun) {
		int v25 = 0;
		int nums = static_cast<int>(num);
		return SYMCALL(bool, MSSYM_B1QE22modifyScoreInObjectiveB1AE21ScoreboardIdentityRefB2AAA4QEAAB1UE18NAEAHAEAVObjectiveB2AAE25HW4PlayerScoreSetFunctionB3AAAA1Z, this, &v25, obj, nums, setfun);
	}
};
struct Scoreboard {
	auto getobject(std::string* str) {
		return SYMCALL(Objective*, MSSYM_MD5_844f126769868c7d0ef42725c3859954, this, str);
	}
	auto getScoreboardId(std::string* str) {
		return SYMCALL(ScoreboardId*, MSSYM_MD5_ecded9d31b4a1c24ba985b0a377bef64, this, str);
	}
	std::vector<Objective*>* getobjects() {
		return SYMCALL(std::vector<Objective*>*, MSSYM_B1QE13getObjectivesB1AE10ScoreboardB2AAA4QEBAB1QA2AVB2QDA6vectorB1AE13PEBVObjectiveB2AAA1VB2QDA9allocatorB1AE13PEBVObjectiveB3AAAA3stdB3AAAA3stdB2AAA2XZ, this);
	}
	auto getDisplayInfoFiltered(std::string* str) {
		return SYMCALL(std::vector<PlayerScore>*, MSSYM_MD5_3b3c17fbee13a54836ae12d93bb0dbae, this, str);
	}
	auto gettrackedscoreID() {
		return SYMCALL(std::vector<ScoreboardId>*, MSSYM_B1QE13getTrackedIdsB1AE10ScoreboardB2AAA4QEBAB1QA2AVB2QDA6vectorB1AE13UScoreboardIdB2AAA1VB2QDA9allocatorB1AE13UScoreboardIdB3AAAA3stdB3AAAA3stdB2AAA2XZ, this);
	}
	auto getIdentityDictionary() {
		return (IdentityDictionary*)((char*)this + 80);//gouzaohanshu
	}
	auto getScoreboardID(Player* a2) {
		return SYMCALL(PlayerScoreboardId*, MSSYM_B1QE15getScoreboardIdB1AE10ScoreboardB2AAE20QEBAAEBUScoreboardIdB2AAA9AEBVActorB3AAAA1Z, this, a2);
	}
	auto getScoreboardID2(void* a2) {
		return SYMCALL(ScoreboardId*, MSSYM_B1QE15getScoreboardIdB1AE10ScoreboardB2AAE20QEBAAEBUScoreboardIdB2AAA9AEBVActorB3AAAA1Z, this, a2);
	}
	auto getScoreboardIdentityRef(ScoreboardId* a2) {
		return SYMCALL(ScoreboardIdentityRef*, MSSYM_B1QE24getScoreboardIdentityRefB1AE10ScoreboardB2AAE29QEAAPEAVScoreboardIdentityRefB2AAE16AEBUScoreboardIdB3AAAA1Z, this, a2);
	}
	//bool请设置为1; a6=0 set,a6=1,add,a6=2,remove
	auto modifiedscores(bool* a2, ScoreboardId* a3, Objective* a4, int a5, unsigned __int8 a6) {
		return SYMCALL(int, MSSYM_B1QE17modifyPlayerScoreB1AE10ScoreboardB2AAA8QEAAHAEAB1UE17NAEBUScoreboardIdB2AAE13AEAVObjectiveB2AAE25HW4PlayerScoreSetFunctionB3AAAA1Z, this, a2, a3, a4, a5, a6);
	}
};

#endif
