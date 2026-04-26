#pragma once

#include <string>
#include <vector>

class StateLog {
public:
	enum ACTION_TYPE {
		AUCTION,
		BANKRUPT, // done
		BUILD_HOTEL, // BELUM IMPLEMENT
		BUILD_HOUSE, // BELUM IMPLEMENT
		BUY_TILE, // done
		CHANCE_CARD, // done
		DICE, // done
		DOUBLE, // done
		FESTIVAL, // BELUM IMPLEMENT
		FUND_CARD, // done
		GET_CARD, // done
		GO_JAIL, // done
		LOAD,
		PAY_MORTGAGE,
		PAY_RENT, // half done, sisa UTILITY
		PAY_TAX, // done
		RAILROAD, // done
		SAVE,
		UTILITY, // done
		SKILL_CARD, // done
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
	Logger();
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
	static Logger& getInstance() {
		static Logger instance;
		return instance;
	}

	std::vector<StateLog> getLogs() const { return logs; }
	void setLogs(std::vector<StateLog> logs) { this->logs = logs; }

	void log(const std::string& username, StateLog::ACTION_TYPE action, const std::string& detail);
	void log(int turn, const std::string& username, StateLog::ACTION_TYPE action, const std::string& detail);
	void printLog(int amount = -1);
};
