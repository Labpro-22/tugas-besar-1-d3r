#include <string>
#include <vector>
#include "../../include/core/Logger.hpp"
#include "../../include/core/GameManager.hpp"
#include <algorithm>
using namespace std;

const std::string StateLog::actionName[] = {
	"AUCTION",
	"BANKRUPT",
	"BUILD_HOTEL",
	"BUILD_HOUSE",
	"BUY_TILE",
	"CHANCE_CARD",
	"DICE",
	"DOUBLE",
	"FESTIVAL",
	"FUND_CARD",
	"GET_CARD",
	"GO_JAIL",
	"LOAD",
	"PAY_MORTGAGE",
	"PAY_RENT",
	"PAY_TAX",
	"RAILROAD",
	"SAVE",
	"UTILITY",
	"SKILL_CARD",
	"WIN"
};

std::string StateLog::actionToString(ACTION_TYPE action){
    return actionName[action];
}

void Logger::log(const std::string& username, StateLog::ACTION_TYPE action, const std::string& detail){
    int turn = GameManager::getInstance().getTurn();
    StateLog newLog = StateLog(turn, username, action, detail);
    logs.push_back(newLog);
}

void Logger::log(int turn, const std::string& username, StateLog::ACTION_TYPE action, const std::string& detail){
    StateLog newLog = StateLog(turn, username, action, detail);
    logs.push_back(newLog);
}
void Logger::printLog(int amount){
    int startIdx;
	GameManager& game = GameManager::getInstance();
    if(amount < 0 || amount > logs.size()){ // nanti pake exception
        startIdx = 0;
        game.writeLine("=== Log Transaksi Penuh ===");
    }
    else{
        startIdx = logs.size() - amount;
        game.writeLine("=== Log Transaksi (" + std::to_string(amount) + " Terakhir) ==="); // tunggu nanti
    }
	game.writeLine("");
    for_each(logs.begin() + startIdx, logs.end(), [&](const StateLog &log){
		game.writeLine("[Turn " + std::to_string(log.getTurn()) + "] "
						+ log.getUsername() + " | "
						+ log.actionToString(log.getAction()) + " | "
						+ log.getDetail());
    });
}