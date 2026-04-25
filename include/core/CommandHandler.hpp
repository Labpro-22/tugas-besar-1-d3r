#pragma once

#include <string>
#include <vector>

class CommandHandler {
public:
    std::string askInput(const std::string& prompt, bool allowEmpty = false) const;
    std::string askChoice(const std::string& prompt, const std::vector<std::string>& validOptions, bool caseInsensitive = true) const;
    int askInt(const std::string& prompt, int minValue, int maxValue) const;

    void commands();
    bool execute(const std::string& line);
};
