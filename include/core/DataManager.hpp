#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

class GameManager;

class DataManager {
private:
    string configMisc;
    string configProperty;
    string configTax;
    string configUtility;
    string configRailroad;
    string configSpecial;

public:
    DataManager(const std::string& configMisc, const std::string& configProperty, const std::string& configTax, const std::string& configUtility, const std::string& configRailroad,const std::string& configSpecial);
    void loadMisc(GameManager& game);
    void loadTax(GameManager& game);
    vector<int> loadUtilityConfig();
    vector<int> loadRailroadConfig();
    void loadProperties(GameManager& game, const vector<int>& utilityRent, const std::vector<int>& railroadRent);
    void loadSpecial(GameManager& game);

    void load(GameManager& game);
    void save(GameManager& game);
};