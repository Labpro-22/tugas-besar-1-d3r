#pragma once

#include <string>

class CommandHandler {
public:
    void commands();
    bool execute(const std::string& line);
};
