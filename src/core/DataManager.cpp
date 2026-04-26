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

void DataManager::loadConfig() {
    loadMisc();

    vector<int> utilityRent = loadUtilityConfig();
    vector<int> railroadRent = loadRailroadConfig();
    vector<int> taxConfig = loadTaxConfig();
    vector<int> specialConfig = loadSpecialConfig();

    loadProperties(utilityRent, railroadRent);
    loadActions(taxConfig, specialConfig);
}

void DataManager::load(string fileName) {
    GameManager& game = GameManager::getInstance();

    game.writeLine("Memuat permainan...");

    if(game.getIsGameLoaded()) {
        throw LoadProhibitedException();
    }
    
    string path = "data/" + fileName;
    if(!isFileExists(path)) {
        throw FileNotExistsException(path);
    }
    ifstream file(path);
    if(!file.is_open()) {
        throw LoadFailedException();
    }

    int turn, maxTurn, playerCount;
    if(!(file >> turn >> maxTurn >> playerCount)) throw LoadFailedException();
    game.setTurn(turn);
    game.setMaxTurn(maxTurn);
    game.setPlayerCount(playerCount);

    int countMove(4), countDiscount(3), countShield(2), countTeleport(2), countLasso(2), countDemolition(2), countFreeJail(2);

    game.writeLine("\tMemuat pemain...");

    // State Pemain
    vector<Player*> players;
    for(int i = 0; i < game.getPlayerCount(); i++) {
        string username;
        int currency;
        string position;
        string statusStr;
        int deckCount;

        if(!(file >> username >> currency >> position >> statusStr >> deckCount)) throw LoadFailedException();
        
        Tile* currentTile = game.getBoard().getTile(position);
        if(currentTile == nullptr) throw LoadFailedException();
        
        PLAYER_STATUS status;
        if(statusStr == "ACTIVE") status = ACTIVE;
        else if(statusStr == "BANKRUPT") status = BANKRUPT;
        else if(statusStr == "JAILED") status = JAILED;
        else throw LoadFailedException();
        
        // Deck
        CardDeck<SkillCard> deck;
        for(int j = 0; j < deckCount; j++) {
            string cardName;
            int cardValue;
            if(!(file >> cardName)) throw LoadFailedException();

            if(cardName == "MOVE_CARD") {
                if(!(file >> cardValue)) throw LoadFailedException();
                deck.addCard(new MoveCard(cardValue));
                countMove--;
            } else if(cardName == "DISCOUNT_CARD") {
                if(!(file >> cardValue)) throw LoadFailedException();
                deck.addCard(new DiscountCard(cardValue));
                countDiscount--;
            } else if(cardName == "SHIELD_CARD") {
                deck.addCard(new ShieldCard());
                countShield--;
            } else if(cardName == "TELEPORT_CARD") {
                deck.addCard(new TeleportCard());
                countTeleport--;
            } else if(cardName == "LASSO_CARD") {
                deck.addCard(new LassoCard());
                countLasso--;
            } else if(cardName == "DEMOLITION_CARD") {
                deck.addCard(new DemolitionCard());
                countDemolition--;
            } else if(cardName == "FREE_JAIL_CARD") {
                deck.addCard(new FreeJailCard());
                countFreeJail--;
            } else {
                throw LoadFailedException();
            }
        }

        players.push_back(new Player(username, currency, currentTile, status, std::move(deck)));

        game.writeLine("\t\tPemain " + players.at(players.size()-1)->getUsername() + " dimuat.");
    }

    game.setPlayers(players);
    game.setCurrentTurnPlayer(players.at(1));

    game.writeLine("\tMemuat properti...");

    // State Property
    int nProperty;
    if(!(file >> nProperty)) throw LoadFailedException();

    for(int i = 0; i < nProperty; i++) {
        string code, type, ownerStr, statusStr;
        int fmult, fdur, nBangunan;
        if(!(file >> code >> type >> ownerStr >> statusStr >> fmult >> fdur >> nBangunan)) throw LoadFailedException();
        
        Player* owner = nullptr;
        if (ownerStr != "BANK") {
            const vector<Player*>& gamePlayers = game.getPlayers();
            for (Player* p : gamePlayers) {
                if (p->getUsername() == ownerStr) {
                    owner = p;
                    break;
                }
            }
            if (owner == nullptr) throw LoadFailedException();
        }

        PROPERTY_STATUS status;
        if(statusStr == "BANK") status = BANK;
        else if (statusStr == "OWNED") status = OWNED;
        else if (statusStr == "MORTGAGED") status = MORTGAGED;
        else throw LoadFailedException();
        
        Tile* tile = game.getBoard().getTile(code);
        if(tile == nullptr) throw LoadFailedException();
        Property* property = dynamic_cast<Property*>(tile);
        if(property == nullptr) throw LoadFailedException();

        property->setOwner(owner);
        property->setPropertyStatus(status);
        property->setFestivalMultiplier(fmult);
        property->setFestivalDuration(fdur);
        
        if(type == "street") {
            Street* streetTile = dynamic_cast<Street*>(property);
            if(streetTile == nullptr) throw LoadFailedException();
            streetTile->setCurrentLevel(nBangunan);
        }
    }

    game.writeLine("\tMemuat deck kartu...");

    // State Deck
    int nSkillCard;
    if(!(file >> nSkillCard)) throw LoadFailedException();

    CardDeck<SkillCard>& skillDeck = game.getSkillDeck();
    for(int i = 0; i < nSkillCard; i++) {
        string cardName;
        if(!(file >> cardName)) throw LoadFailedException(); 

        if(cardName == "MOVE_CARD") {
            skillDeck.addCard(new MoveCard());
            countMove--;
        } else if(cardName == "DISCOUNT_CARD") {
            skillDeck.addCard(new DiscountCard());
            countDiscount--;
        } else if(cardName == "SHIELD_CARD") {
            skillDeck.addCard(new ShieldCard());
            countShield--;
        } else if(cardName == "TELEPORT_CARD") {
            skillDeck.addCard(new TeleportCard());
            countTeleport--;
        } else if(cardName == "LASSO_CARD") {
            skillDeck.addCard(new LassoCard());
            countLasso--;
        } else if(cardName == "DEMOLITION_CARD") {
            skillDeck.addCard(new DemolitionCard());
            countDemolition--;
        } else if(cardName == "FREE_JAIL_CARD") {
            skillDeck.addCard(new FreeJailCard());
            countFreeJail--;
        } else {
            throw LoadFailedException();
        }
    }

    if(countMove < 0 || countDiscount < 0 || countShield < 0 || countTeleport < 0 || countLasso < 0 || countDemolition < 0 || countFreeJail < 0) throw LoadFailedException();

    for(int j = 0; j < countMove; j++) skillDeck.addUsedCard(new MoveCard());
    for(int j = 0; j < countDiscount; j++) skillDeck.addUsedCard(new DiscountCard());
    for(int j = 0; j < countShield; j++) skillDeck.addUsedCard(new ShieldCard());
    for(int j = 0; j < countTeleport; j++) skillDeck.addUsedCard(new TeleportCard());
    for(int j = 0; j < countLasso; j++) skillDeck.addUsedCard(new LassoCard());
    for(int j = 0; j < countDemolition; j++) skillDeck.addUsedCard(new DemolitionCard());
    for(int j = 0; j < countFreeJail; j++) skillDeck.addUsedCard(new FreeJailCard());

    game.writeLine("\tMemuat log...");

    // State Log
    int nLog;
    if(!(file >> nLog)) throw LoadFailedException();

    if(nLog <= 0) return;

    std::unordered_map<std::string, StateLog::ACTION_TYPE> mapStringToAction;
    for (int i = 0; i < 21; i++) {
        StateLog::ACTION_TYPE action = static_cast<StateLog::ACTION_TYPE>(i);
        mapStringToAction[StateLog::actionToString(action)] = action;
    }

    Logger& logger = game.getLogger();
    for(int i = 0; i < nLog; i++) {
        int turn;
        string username, actionTypeStr, detail;
        if(!(file >> turn >> username >> actionTypeStr)) throw LoadFailedException();
        getline(file, detail);

        if(!detail.empty() && detail[0] == ' ') detail.erase(0, 1);
        
        auto it = mapStringToAction.find(actionTypeStr);
        if(it == mapStringToAction.end()) {
            throw LoadFailedException();
        }
        StateLog::ACTION_TYPE action = it->second;

        logger.log(turn, username, action, detail);
    }

    if (game.getCurrentTurnPlayer() != nullptr) {
        logger.log(
            game.getCurrentTurnPlayer()->getUsername(),
            StateLog::LOAD,
            "Memuat permainan dari " + path
        );
    }

    file.close();
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
    
    // State Player
    const vector<Player*>& players = game.getPlayers();
    file << players.size() << "\n";
    for(size_t i = 0; i < players.size(); i++) {
        const Player* player = players.at(i);
        file << player->getUsername() << " " << player->getCurrency() << " " << player->getCurrentTile()->getCode() << " ";
        
        PLAYER_STATUS status = player->getStatus();
        if(status == ACTIVE) file << "ACTIVE\n";
        else if(status == BANKRUPT) file << "BANKRUPT\n";
        else if(status == JAILED) file << "JAILED\n";
        else throw SaveFailedException();
        
        const vector<SkillCard*>& skillCards = player->getDeck().getCards();
        file << skillCards.size() << "\n";
        for(size_t i = 0; i < skillCards.size(); i++) {
            const SkillCard* skillCard = skillCards.at(i);
            file << skillCard->getCardName();
            if(skillCard->getCardValue() != 0) {
                file << " " << skillCard->getCardValue();
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
        
        PROPERTY_STATUS status = street->getPropertyStatus();
        if(status == BANK) file << "BANK";
        else if(status == OWNED) file << "OWNED";
        else if(status == MORTGAGED) file << "MORTGAGED";
        else throw SaveFailedException();
        
        file << " " << street->getFestivalMultiplier() << " " << street->getFestivalDuration() << " " << street->getCurrentLevel() << "\n";
    }

    for(size_t i = 0; i < railroads.size(); i++) {
        const Railroad* railroad = railroads.at(i);
        file << railroad->getCode() << " " << "railroad" << " ";
        if(railroad->getOwner() == nullptr) file << "BANK" << " ";
        else file << railroad->getOwner()->getUsername() << " ";

        PROPERTY_STATUS status = railroad->getPropertyStatus();
        if(status == BANK) file << "BANK";
        else if(status == OWNED) file << "OWNED";
        else if(status == MORTGAGED) file << "MORTGAGED";
        else throw SaveFailedException();

        file << " " << railroad->getFestivalMultiplier() << " " << railroad->getFestivalDuration() << " " << 0 << "\n";
    }

    for(size_t i = 0; i < utilities.size(); i++) {
        const Utility* utility = utilities.at(i);
        file << utility->getCode() << " " << "utility" << " ";
        if(utility->getOwner() == nullptr) file << "BANK" << " ";
        else file << utility->getOwner()->getUsername() << " ";

        PROPERTY_STATUS status = utility->getPropertyStatus();
        if(status == BANK) file << "BANK";
        else if(status == OWNED) file << "OWNED";
        else if(status == MORTGAGED) file << "MORTGAGED";
        else throw SaveFailedException();
        
        file << " " << utility->getFestivalMultiplier() << " " << utility->getFestivalDuration() << " " << 0 << "\n";
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
        file << log.getTurn() << " " << log.getUsername() << " " << log.actionToString(log.getAction()) << " " << log.getDetail() << "\n";
    }

    if (game.getCurrentTurnPlayer() != nullptr) {
        game.getLogger().log(
            game.getCurrentTurnPlayer()->getUsername(),
            StateLog::SAVE,
            "Menyimpan permainan ke " + path
        );
    }

    file.close();
}

bool DataManager::isFileExists (const string& name) {
    struct stat buffer;   
    return (stat(name.c_str(), &buffer) == 0); 
}
