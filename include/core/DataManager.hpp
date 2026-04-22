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
    string configAction;

public:
    DataManager(const std::string& configMisc, const std::string& configProperty, const std::string& configTax, const std::string& configUtility, const std::string& configRailroad,const std::string& configSpecial);
    DataManager(const std::string& configMisc, const std::string& configProperty, const std::string& configTax, const std::string& configUtility, const std::string& configRailroad,const std::string& configSpecial, const std::string& configAction);
    void loadMisc(GameManager& game);
    std::vector<int> loadTaxConfig();
    vector<int> loadUtilityConfig();
    vector<int> loadRailroadConfig();
    void loadProperties(GameManager& game, const vector<int>& utilityRent, const std::vector<int>& railroadRent);
    vector<int> loadSpecialConfig();
    void loadActions(GameManager& game, const vector<int>& taxConfig, const vector<int>& specialConfig);
    void loadDefaultTiles();

    void load(GameManager& game);
    void load();
    void save(GameManager& game);
};
