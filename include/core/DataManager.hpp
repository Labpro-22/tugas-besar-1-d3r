#pragma once

#include <string>

class GameManager;

class DataManager {
private:
    std::string configMisc;
    std::string configProperty;
    std::string configTax;
    std::string configUtility;
    std::string configRailroad;

public:
    DataManager(const std::string&, const std::string&, const std::string&, const std::string&, const std::string&);

    void load(GameManager& game);
    void save(GameManager& game);
};