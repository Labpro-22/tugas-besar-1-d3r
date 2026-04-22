#pragma once

#include "../../include/core/DataManager.hpp"
#include "../../include/core/GameManager.hpp"


DataManager::DataManager(const std::string& configMisc, const std::string& configProperty, const std::string& configTax, const std::string& configUtility, const std::string& configRailroad, const std::string& configSpecial) : configMisc(configMisc), configProperty(configProperty), configTax(configTax), configUtility(configUtility), configRailroad(configRailroad), configSpecial(configSpecial){}

void DataManager::loadMisc(GameManager& game) {
    ifstream file(configMisc);
    if (!file.is_open()) {       
        return;
    }

    string headerMaxturn, headerSaldoAwal;
    int tokenMaxturn, tokenSaldoAwal;

    file >> headerMaxturn >> headerSaldoAwal;
    file >> tokenMaxturn >> tokenSaldoAwal;  
    
    game.setMaxTurn(tokenMaxturn);
    game.setAllPlayersCurrency(tokenSaldoAwal);
    file.close();
}

void DataManager::loadProperties(GameManager& game, const vector<int>& utilityRent, const std::vector<int>& railroadRent) {
    ifstream file(configProperty);
    if (!file.is_open()) {       
        return;
    }

    string header;
    getline(file, header);

    int id, landCost, mortgageValue, houseCost, hotelCost;
    string code, name, type, color;
    while (file >> id >> code >> name >> type >> color >> landCost >> mortgageValue >> houseCost >> hotelCost) {
        vector<int> rentCost(6);
        for (int i = 0; i < 6; i++){
            file >> rentCost[i];
        }        

        if (type == "STREET") {
            Street* streetTile = new Street(id,code,color,landCost,mortgageValue,1,0,nullptr,BANK,houseCost,hotelCost,rentCost,0);
            game.addTile(streetTile);
        } else if (type == "RAILROAD"){
            Railroad* railroadTile = new Railroad(id,code,color,landCost,mortgageValue,1,0,nullptr,BANK,railroadRent);
            game.addTile(railroadTile);
        } else if (type == "UTILITY"){
            Utility* utilityTile = new Utility(id,code,color,landCost,mortgageValue,1,0,nullptr,BANK,utilityRent);
            game.addTile(utilityTile);
        }
    }
    file.close();
}



vector<int> DataManager::loadRailroadConfig() {
    ifstream file(configRailroad);
    if (!file.is_open()) {       
        return vector<int>();
    }

    vector<int> mortgageRange;
    string header;
    getline(file, header);    

    int totalRailRoad, mortageVal;
    while (file >> totalRailRoad >> mortageVal) {
        mortgageRange.push_back(mortageVal);
    }
    file.close();
    return mortgageRange;
}

void DataManager::loadTax(GameManager& game) {
    ifstream file(configTax);
    if (!file.is_open()) {       
        return;
    }    
    string header;
    getline(file, header);

    int pphFlat, pphPercentage, pbmFlat;
        
    file >> pphFlat >> pphPercentage >> pbmFlat;

    PPH* pph = new PPH(5, "PPH", "DEFAULT", pphFlat, pphPercentage);
    PBM* pbm = new PBM(39, "PBM", "DEFAULT", pbmFlat);    

    game.addTile(pph);
    game.addTile(pbm);
    file.close();
}


vector<int> DataManager::loadUtilityConfig() {
    ifstream file(configUtility);
    if (!file.is_open()) {       
        return vector<int>();
    }    
    string header;
    getline(file, header);

        
    vector<int> multiplyFactor;

    int totalUtility, factor;
    while (file >> totalUtility >> factor){
        multiplyFactor.push_back(factor);
    }

    return multiplyFactor;
    
} 

 void DataManager::loadSpecial(GameManager& game) {
    ifstream file(configSpecial);
    if (!file.is_open()) {       
        return;
    }    
    string header;
    getline(file, header);

    int goSalary, jail_fine;

    file >> goSalary >> jail_fine;    

    Go* go = new Go(1, "GO", "DEFAULT", goSalary);
    Prison* prison = new Prison(11, "PEN", "DEFAULT", jail_fine);

    game.addTile(go);
    game.addTile(prison);
    file.close();
}
 
void DataManager::load(GameManager& game) {
    loadMisc(game);

    vector<int> utilityRent = loadUtilityConfig();
    vector<int> railroadRent = loadRailroadConfig();

    loadProperties(game, utilityRent, railroadRent);
    loadTax(game);
    loadSpecial(game);
}