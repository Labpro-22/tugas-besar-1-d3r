#pragma once

#include <string>
#include <vector>

class StateLog {
public:
	enum ACTION_TYPE {
		BUY,
		DICE,
		USECARD,
		GETCARD,
		PAYRENT,
		PAYTAX
	};

private:
	int turn;
	std::string username;
	ACTION_TYPE action;
	std::string detail;

public:
	StateLog(int, const std::string&, ACTION_TYPE, const std::string&);

	int getTurn() const;
	std::string getUsername() const;
	ACTION_TYPE getAction() const;
	std::string getDetail() const;
};

class Logger {
private:
	static std::vector<StateLog> logs;

public:
	static void log(int, const std::string&, StateLog::ACTION_TYPE, const std::string&);
	static void printLog();
};
