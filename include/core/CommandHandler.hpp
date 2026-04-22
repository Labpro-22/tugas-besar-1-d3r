#pragma once

#include <string>

class CommandHandler {
public:
    std::string askInput(const std::string& prompt) const;
    int askInt(const std::string& prompt, int minValue, int maxValue) const;

    void commands();
    bool execute(const std::string& line);
};
