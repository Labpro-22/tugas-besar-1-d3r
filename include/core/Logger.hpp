#pragma once

#include <string>
#include <vector>

class StateLog {
	public:
	enum ACTION_TYPE {
		AUCTION,
		BANKRUPT, // done
		BUILD_HOTEL,
		BUILD_HOUSE,
		BUY_TILE,
		CHANCE_CARD,
		DICE, // done
		DOUBLE, // done
		FESTIVAL,
		FUND_CARD,
		GET_CARD,
		GO_JAIL, // done
		LOAD,
		PAY_MORTGAGE,
		PAY_RENT,
		PAY_TAX,
		RAILROAD,
		SAVE,
		UTILITY,
		USE_CARD,
		WIN
	};

private:
	static const std::string actionName[];
	int turn;
	std::string username;
	ACTION_TYPE action;
	std::string detail;

public:
	StateLog(int turn, const std::string& username, ACTION_TYPE action, const std::string& detail)
	: turn(turn), username(username), action(action), detail(detail) {}

	int getTurn() const { return turn; }
	std::string getUsername() const { return username; }
	ACTION_TYPE getAction() const { return action; }
	std::string getDetail() const { return detail; }
	static std::string actionToString(ACTION_TYPE action);
};

class Logger {
private:
	std::vector<StateLog> logs;

public:
	static Logger& getInstance() {
		static Logger instance;
		return instance;
	}
	void log(const std::string& username, StateLog::ACTION_TYPE action, const std::string& detail);
	void log(int turn, const std::string& username, StateLog::ACTION_TYPE action, const std::string& detail);
	void printLog(int amount = -1);
};
