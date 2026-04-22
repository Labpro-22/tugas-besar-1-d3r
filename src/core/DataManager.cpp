#include "../../include/core/DataManager.hpp"
#include "../../include/core/GameManager.hpp"


DataManager::DataManager(const std::string& configMisc, const std::string& configProperty, const std::string& configTax, const std::string& configUtility, const std::string& configRailroad, const std::string& configSpecial) : configMisc(configMisc), configProperty(configProperty), configTax(configTax), configUtility(configUtility), configRailroad(configRailroad), configSpecial(configSpecial), configAction("config/aksi.txt"){}

DataManager::DataManager(const std::string& configMisc, const std::string& configProperty, const std::string& configTax, const std::string& configUtility, const std::string& configRailroad, const std::string& configSpecial, const std::string& configAction) : configMisc(configMisc), configProperty(configProperty), configTax(configTax), configUtility(configUtility), configRailroad(configRailroad), configSpecial(configSpecial), configAction(configAction){}

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
            Street* streetTile = new Street(id - 1, code, name, color, landCost, mortgageValue, 1, 0, nullptr, BANK, houseCost, hotelCost, rentCost, 0);
            game.addTile(streetTile);
        } else if (type == "RAILROAD"){
            Railroad* railroadTile = new Railroad(id - 1, code, name, color, landCost, mortgageValue, 1, 0, nullptr, BANK, railroadRent);
            game.addTile(railroadTile);
        } else if (type == "UTILITY"){
            Utility* utilityTile = new Utility(id - 1, code, name, color, landCost, mortgageValue, 1, 0, nullptr, BANK, utilityRent);
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

vector<int> DataManager::loadTaxConfig() {
    ifstream file(configTax);
    if (!file.is_open()) {       
        return vector<int>();
    }    
    string header;
    getline(file, header);

    int pphFlat, pphPercentage, pbmFlat;
        
    file >> pphFlat >> pphPercentage >> pbmFlat;
    file.close();
    return {pphFlat, pphPercentage, pbmFlat};
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

    file.close();
    return multiplyFactor;
    
} 

vector<int> DataManager::loadSpecialConfig() {
    ifstream file(configSpecial);
    if (!file.is_open()) {       
        return vector<int>();
    }    
    string header;
    getline(file, header);

    int goSalary, jail_fine;

    file >> goSalary >> jail_fine;    
    file.close();
    return {goSalary, jail_fine};
}

void DataManager::loadActions(GameManager& game, const vector<int>& taxConfig, const vector<int>& specialConfig) {
    ifstream file(configAction);
    if (!file.is_open()) {
        return;
    }

    const int pphFlat = taxConfig.size() > 0 ? taxConfig[0] : 0;
    const int pphPercentage = taxConfig.size() > 1 ? taxConfig[1] : 0;
    const int pbmFlat = taxConfig.size() > 2 ? taxConfig[2] : 0;
    const int goSalary = specialConfig.size() > 0 ? specialConfig[0] : 0;
    const int jailFine = specialConfig.size() > 1 ? specialConfig[1] : 0;

    int id;
    string code, name, type, color;
    while (file >> id >> code >> name >> type >> color) {
        const int index = id - 1;

        if (type == "KARTU") {
            game.addTile(new CardTile(index, code, name, color));
        } else if (type == "FESTIVAL") {
            game.addTile(new Festival(index, code, name, color));
        } else if (type == "PAJAK") {
            if (code == "PPH") {
                game.addTile(new PPH(index, code, color, pphFlat, pphPercentage));
            } else if (code == "PBM") {
                game.addTile(new PBM(index, code, name, color, pbmFlat));
            }
        } else if (type == "SPESIAL") {
            if (code == "GO") {
                game.addTile(new Go(index, code, name, color, goSalary));
            } else if (code == "PEN") {
                game.addTile(new Prison(index, code, name, color, jailFine));
            } else if (code == "BBP") {
                game.addTile(new FreeParking(index, code, name, color));
            } else if (code == "PPJ") {
                game.addTile(new Trap(index, code, name, color));
            }
        }
    }
    file.close();
}

void DataManager::loadDefaultTiles() {
    // GameManager& game = GameManager::getInstance();

    // for (int id = 1; id <= 40; id++) {
    //     int index = id - 1;
    //     if (game.getBoard().getTile(index) == nullptr) {
    //         game.addTile(new CardTile(index, "ACT" + to_string(id), "Petak Aksi", "DEFAULT"));
    //     }
    // }
}
 
void DataManager::load(GameManager& game) {
    loadMisc(game);

    vector<int> utilityRent = loadUtilityConfig();
    vector<int> railroadRent = loadRailroadConfig();
    vector<int> taxConfig = loadTaxConfig();
    vector<int> specialConfig = loadSpecialConfig();

    loadProperties(game, utilityRent, railroadRent);
    loadActions(game, taxConfig, specialConfig);
    loadDefaultTiles();
}

void DataManager::load() {
    load(GameManager::getInstance());
}
