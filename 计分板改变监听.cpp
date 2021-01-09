using namespace std;
struct Player {
	
};
struct Scoreboard {
	auto getScoreboardId(std::string* str) {
		return SYMCALL(ScoreboardId*, MSSYM_MD5_ecded9d31b4a1c24ba985b0a377bef64, this, str);
	auto getScoreboardID(Player* a2) {
		return SYMCALL(ScoreboardId*, MSSYM_B1QE15getScoreboardIdB1AE10ScoreboardB2AAE20QEBAAEBUScoreboardIdB2AAA9AEBVActorB3AAAA1Z, this, a2);
	}
};
struct ScoreboardId {
	int id;
	//
};
struct ScoreInfo {
	//从 scoreboard::getscores得到
	auto getcount() {
		return *(int*)((VA)(this) + 12);
	}
};
struct Objective {
	//从objective::objective得到
	//获取计分板名称
	auto getscorename() {
		return *(std::string*)((VA)(this) + 64);
	}
	//获取计分板展示名称
	auto getscoredisplayname() {
		return *(std::string*)((VA)(this) + 96);
	}
	auto getscoreinfo(ScoreboardId* a2) {
		char a1[12];
		return SYMCALL(ScoreInfo*, MSSYM_B1QE14getPlayerScoreB1AA9ObjectiveB2AAA4QEBAB1QE11AUScoreInfoB2AAE16AEBUScoreboardIdB3AAAA1Z, this, a1, a2);
	}
};

//==============================================辅助加载============================================================================================
Scoreboard* scoreboard;//储存计分板指针
static std::map<int, Player*> player_socreboardid;//储存玩家的计分板id
//在服务器开始时获取计分板指针
THook(void*, MSSYM_B2QQE170ServerScoreboardB2AAA4QEAAB1AE24VCommandSoftEnumRegistryB2AAE16PEAVLevelStorageB3AAAA1Z, void* _this, void* a2, void* a3) {
	scoreboard = (Scoreboard*)original(_this, a2, a3);
	return scoreboard;
}
// [原型] public: virtual void __cdecl ServerScoreboard::onPlayerJoined(class Player const & __ptr64) __ptr64
// [符号] ?onPlayerJoined@ServerScoreboard@@UEAAXAEBVPlayer@@@Z
//玩家加入时获取玩家的计分板ID
THook(void, MSSYM_B1QE14onPlayerJoinedB1AE16ServerScoreboardB2AAE15UEAAXAEBVPlayerB3AAAA1Z, Scoreboard* class_this, Player* player) {
	//在玩家加入时存储玩家指针，方便通过计分板id获取玩家指针
	//如需玩家离开时删除，请自行编写
	int playersocreboardid = landmoney_scoreboard->getScoreboardID(player)->id;
	player_socreboardid[playersocreboardid] = player;
	original(class_this, player);
}
/*
玩家离开时：{
	player_socreboardid.erase(playersocreboardid);
}*/

//================================封装函数===================================================================
//指令（/scoreboard players <add|remove|set> <playersname> <objectivename> <playersnum>）中<playersname>对应计分板id
//在使用时当玩家不在线使用的是第一个，在线时使用第二个

//通过计分板名称获取计分板id
int getScoreBoardId_byString(std::string* str) {
	scoreboard->getScoreboardId(str);
}
//通过玩家指针获取计分板id
int getScoreBoardId_byPlayer(Player* player) {
	scoreboard->getScoreboardID(player);
}
//===================================================将以下设置为监听======================================================================================
// [原型] public: virtual void __cdecl ServerScoreboard::onScoreChanged(struct ScoreboardId const & __ptr64,class Objective const & __ptr64) __ptr64
// [符号] ?onScoreChanged@ServerScoreboard@@UEAAXAEBUScoreboardId@@AEBVObjective@@@Z
// 计分板改变时的监听
THook(void, MSSYM_B1QE14onScoreChangedB1AE16ServerScoreboardB2AAE21UEAAXAEBUScoreboardIdB2AAE13AEBVObjectiveB3AAAA1Z, const struct Scoreboard* class_this, ScoreboardId* a2, Objective* a3)
{
	/*
	原命令：
	创建计分板时：/scoreboard objectives <add|remove> <objectivename> dummy <objectivedisplayname>
	修改计分板时（此函数hook此处)：/scoreboard players <add|remove|set> <playersname> <objectivename> <playersnum>
	*/
	int scoreboardid = a2->id;
	Player* player = player_socreboardid[scoreboardid];
	if (player) {
		cout << player->getNameTag() << endl;//获取 <playersname>
	}
	cout << to_string(scoreboardid) << endl;//获取计分板id
	cout << to_string(a3->getscoreinfo(a2)->getcount()) << endl;//获取修改后的<playersnum>
	cout << a3->getscorename() << endl;//获取<objectivename>
	cout << a3->getscoredisplayname() << endl;//获取<objectivedisname>
	original(class_this, a2, a3);
}
