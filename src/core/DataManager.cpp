#include "../../include/core/DataManager.hpp"
#include "../../include/core/GameManager.hpp"
#include "../../include/core/Tile.hpp"
#include "../../include/utils/GameException.hpp"
#include <sys/stat.h>


DataManager::DataManager(const std::string& configMisc, const std::string& configProperty, const std::string& configTax, const std::string& configUtility, const std::string& configRailroad, const std::string& configSpecial) : configMisc(configMisc), configProperty(configProperty), configTax(configTax), configUtility(configUtility), configRailroad(configRailroad), configSpecial(configSpecial), configAction("config/aksi.txt"){}

DataManager::DataManager(const std::string& configMisc, const std::string& configProperty, const std::string& configTax, const std::string& configUtility, const std::string& configRailroad, const std::string& configSpecial, const std::string& configAction) : configMisc(configMisc), configProperty(configProperty), configTax(configTax), configUtility(configUtility), configRailroad(configRailroad), configSpecial(configSpecial), configAction(configAction){}

void DataManager::loadMisc() {
    try {
        ifstream file(configMisc);
        if (!file.is_open()) {
            throw FileNotExistsException(configMisc);
        }

        string headerMaxturn, headerSaldoAwal;
        int tokenMaxturn, tokenSaldoAwal;

        if (!(file >> headerMaxturn >> headerSaldoAwal >> tokenMaxturn >> tokenSaldoAwal)) {
            throw LoadFailedException();
        }

        GameManager& game = GameManager::getInstance();
        game.setMaxTurn(tokenMaxturn);
        game.setAllPlayersCurrency(tokenSaldoAwal);
        file.close();
    } catch (const NimonspoliException&) {
        return; 
    }
}

void DataManager::loadProperties(const vector<int>& utilityRent, const std::vector<int>& railroadRent) {
    try {
        ifstream file(configProperty);
        if (!file.is_open()) {
            throw FileNotExistsException(configProperty);
        }

        string header;
        if (!getline(file, header)) {
            throw LoadFailedException();
        }

        int id, landCost, mortgageValue, houseCost, hotelCost;
        string code, name, type, color;
        GameManager& game = GameManager::getInstance();
        while (file >> id >> code >> name >> type >> color >> landCost >> mortgageValue >> houseCost >> hotelCost) {
            vector<int> rentCost(6);
            for (int i = 0; i < 6; i++){
                file >> rentCost[i];
            }

            if (type == "STREET") {
                Street* streetTile = new Street(id, code, color,name, landCost, mortgageValue, 1, 0, nullptr, BANK, houseCost, hotelCost, rentCost, 0);
                game.addTile(streetTile);
            } else if (type == "RAILROAD"){
                Railroad* railroadTile = new Railroad(id, code, name, color, landCost, mortgageValue, 1, 0, nullptr, BANK, railroadRent);
                game.addTile(railroadTile);
            } else if (type == "UTILITY"){
                Utility* utilityTile = new Utility(id, code, name, color, landCost, mortgageValue, 1, 0, nullptr, BANK, utilityRent);
                game.addTile(utilityTile);
            }
        }

        file.close();
    } catch (const NimonspoliException&) {
        return;
    }
}



vector<int> DataManager::loadRailroadConfig() {
    try {
        ifstream file(configRailroad);
        if (!file.is_open()) {
            throw FileNotExistsException(configRailroad);
        }

        vector<int> mortgageRange;
        string header;
        if (!getline(file, header)) {
            throw LoadFailedException();
        }

        int totalRailRoad, mortageVal;
        while (file >> totalRailRoad >> mortageVal) {
            mortgageRange.push_back(mortageVal);
        }
        file.close();
        return mortgageRange;
    } catch (const NimonspoliException&) {
        return vector<int>();
    }
}

vector<int> DataManager::loadTaxConfig() {
    try {
        ifstream file(configTax);
        if (!file.is_open()) {
            throw FileNotExistsException(configTax);
        }

        string header;
        if (!getline(file, header)) {
            throw LoadFailedException();
        }

        int pphFlat, pphPercentage, pbmFlat;
        if (!(file >> pphFlat >> pphPercentage >> pbmFlat)) {
            throw LoadFailedException();
        }

        file.close();
        return {pphFlat, pphPercentage, pbmFlat};
    } catch (const NimonspoliException&) {
        return vector<int>();
    }
}


vector<int> DataManager::loadUtilityConfig() {
    try {
        ifstream file(configUtility);
        if (!file.is_open()) {
            throw FileNotExistsException(configUtility);
        }

        string header;
        if (!getline(file, header)) {
            throw LoadFailedException();
        }


        vector<int> multiplyFactor;

        int totalUtility, factor;
        while (file >> totalUtility >> factor){
            multiplyFactor.push_back(factor);
        }

        file.close();
        return multiplyFactor;
    } catch (const NimonspoliException&) {
        return vector<int>();
    }
} 

vector<int> DataManager::loadSpecialConfig() {
    try {
        ifstream file(configSpecial);
        if (!file.is_open()) {
            throw FileNotExistsException(configSpecial);
        }

        string header;
        if (!getline(file, header)) {
            throw LoadFailedException();
        }

        int goSalary, jail_fine;

        if (!(file >> goSalary >> jail_fine)) {
            throw LoadFailedException();
        }
        file.close();
        return {goSalary, jail_fine};
    } catch (const NimonspoliException&) {
        return vector<int>();
    }
}

void DataManager::loadActions(const vector<int>& taxConfig, const vector<int>& specialConfig) {
    try {
        ifstream file(configAction);
        if (!file.is_open()) {
            throw FileNotExistsException(configAction);
        }

        const int pphFlat = taxConfig.size() > 0 ? taxConfig[0] : 0;
        const int pphPercentage = taxConfig.size() > 1 ? taxConfig[1] : 0;
        const int pbmFlat = taxConfig.size() > 2 ? taxConfig[2] : 0;
        const int goSalary = specialConfig.size() > 0 ? specialConfig[0] : 0;
        const int jailFine = specialConfig.size() > 1 ? specialConfig[1] : 0;

        int id;
        string code, name, type, color;
        GameManager& game = GameManager::getInstance();
        while (file >> id >> code >> name >> type >> color) {
            if (type == "KARTU") {
                game.addTile(new CardTile(id, code, name, color));
            } else if (type == "FESTIVAL") {
                game.addTile(new Festival(id, code, name, color));
            } else if (type == "PAJAK") {
                if (code == "PPH") {
                    game.addTile(new PPH(id, code, color, pphFlat, pphPercentage));
                } else if (code == "PBM") {
                    game.addTile(new PBM(id, code, name, color, pbmFlat));
                }
            } else if (type == "SPESIAL") {
                if (code == "GO") {
                    game.addTile(new Go(id, code, name, color, goSalary));
                } else if (code == "PEN") {
                    game.addTile(new Prison(id, code, name, color, jailFine));
                } else if (code == "BBP") {
                    game.addTile(new FreeParking(id, code, name, color));
                } else if (code == "PPJ") {
                    game.addTile(new Trap(id, code, name, color));
                }
            }
        }

        file.close();
    } catch (const NimonspoliException&) {
        return;
    }
}

void DataManager::load() {
    loadMisc();

    vector<int> utilityRent = loadUtilityConfig();
    vector<int> railroadRent = loadRailroadConfig();
    vector<int> taxConfig = loadTaxConfig();
    vector<int> specialConfig = loadSpecialConfig();

    loadProperties(utilityRent, railroadRent);
    loadActions(taxConfig, specialConfig);
}

void DataManager::save(string fileName, bool override) {
    GameManager& game = GameManager::getInstance();
    
    const std::vector<StateLog>& logs = game.getLogger().getLogs();
    if(!logs.empty() && logs.back().getTurn() == game.getTurn()) {
        throw SaveProhibitedException();
    }

    string path = "data/" + fileName;
    if(isFileExists(path) && !override) {
        throw FileExistsException(path);
    }
    ofstream file("data/" + fileName);
    if (!file.is_open()) {
        throw SaveFailedException();
    }

    file << game.getTurn() << " " << game.getMaxTurn() << "\n";
    const vector<Player*>& players = game.getPlayers();

    // State Player
    file << players.size() << "\n";
    for(size_t i = 0; i < players.size(); i++) {
        const Player* player = players.at(i);
        file << player->getUsername() << " " << player->getCurrency() << " " << player->getCurrentTile()->getCode() << " " << player->getStatus() << "\n";
        const vector<SkillCard*>& skillCards = player->getDeck().getCards();
        for(size_t i = 0; i < skillCards.size(); i++) {
            const SkillCard* skillCard = skillCards.at(i);
            file << skillCard->getCardName();
            if(skillCard->getCardValue() != 0) {
                file << " " << skillCard->getCardValue();
            }
            if(skillCard->getCardDuration() != 0) {
                file << " " << skillCard->getCardDuration();
            }
            file << "\n";
        }
    }

    // State Properti
    const vector<Street*> &streets = game.getBoard().getAllStreet();
    const vector<Railroad*> &railroads = game.getBoard().getAllRailroad();
    const vector<Utility*> &utilities = game.getBoard().getAllUtility();

    file << streets.size() + railroads.size() + utilities.size() << "\n";

    for(size_t i = 0; i < streets.size(); i++) {
        const Street* street = streets.at(i);
        file << street->getCode() << " " << "street" << " ";
        if(street->getOwner() == nullptr) file << "BANK" << " ";
        else file << street->getOwner()->getUsername() << " ";
        file << street->getPropertyStatus() << " " << street->getFestivalMultiplier() << " " << street->getFestivalDuration() << " " << street->getCurrentLevel() << "\n";
    }

    for(size_t i = 0; i < railroads.size(); i++) {
        const Railroad* railroad = railroads.at(i);
        file << railroad->getCode() << " " << "railroad" << " ";
        if(railroad->getOwner() == nullptr) file << "BANK" << " ";
        else file << railroad->getOwner()->getUsername() << " ";
        file << railroad->getPropertyStatus() << " " << railroad->getFestivalMultiplier() << " " << railroad->getFestivalDuration() << " " << 0 << "\n";
    }

    for(size_t i = 0; i < utilities.size(); i++) {
        const Utility* utility = utilities.at(i);
        file << utility->getCode() << " " << "utility" << " ";
        if(utility->getOwner() == nullptr) file << "BANK" << " ";
        else file << utility->getOwner()->getUsername() << " ";
        file << utility->getPropertyStatus() << " " << utility->getFestivalMultiplier() << " " << utility->getFestivalDuration() << " " << 0 << "\n";
    }

    // State Deck
    const CardDeck<SkillCard>& skillDeck = game.getSkillDeck();

    file << skillDeck.size() << "\n";

    for(size_t i = 0; i < skillDeck.size(); i++) {
        const SkillCard* skillCard = skillDeck.getCards().at(i);
        file << skillCard->getCardName() << "\n";
    }

    // State Log
    file << logs.size() << "\n";

    for(size_t i = 0; i < logs.size(); i++) {
        const StateLog& log = logs.at(i);
        file << log.getTurn() << " " << log.getUsername() << " " << log.getAction() << " " << log.getDetail() << "\n";
    }

    file.close();
}

bool DataManager::isFileExists (const string& name) {
    struct stat buffer;   
    return (stat(name.c_str(), &buffer) == 0); 
}
