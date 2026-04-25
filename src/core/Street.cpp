#include "../../include/core/Tile.hpp"
#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Logger.hpp"

using namespace std;

Street::Street(int index, const std::string& code, const std::string& color,
    const std::string& name, int landCost, int mortgageValue, 
    int festivalMultiplier, int festivalDuration, 
    Player* owner, PROPERTY_STATUS propertyStatus,
     int houseCost, int hotelCost, 
     std::vector<int> rentCost, int currentLevel) : 
     Property(index, code,  name,color, landCost, mortgageValue, festivalMultiplier, festivalDuration, owner, propertyStatus), houseCost(houseCost), hotelCost(hotelCost), rentCost(rentCost), currentLevel(currentLevel) {} 

int Street::getHouseCost() const {
    return houseCost;
}
int Street::getHotelCost() const {
    return hotelCost;
}
int Street::getCurrentLevel() const {
    return currentLevel;
}
void Street::setCurrentLevel(int currentLevel) {
    this->currentLevel = currentLevel;
}

int Street::getBuildCost() const {
    if (currentLevel >= 4) {
        return hotelCost;
    }
    return houseCost;
}

bool Street::canBuild(Player& player) const {
    if (this->getOwner() != &player) {
        return false;
    }
    if (this->getPropertyStatus() != OWNED) {
        return false;
    }
    if (currentLevel >= 5) {
        return false;
    }

    return GameManager::getInstance().getBoard().canUpgradeProperty(player, const_cast<Street*>(this));
}

bool Street::build(Player& player) {
    if (!canBuild(player)) {
        return false;
    }

    const int cost = getBuildCost();
    if (player.getCurrency() < cost) {
        return false;
    }

    const bool toHotel = (currentLevel == 4);
    player -= cost;
    currentLevel++;

    Logger& logger = Logger::getInstance();
    if (toHotel) {
        logger.log(player.getUsername(), StateLog::BUILD_HOTEL, "Upgrade hotel di " + name + " (" + code + ")");
    } else {
        logger.log(player.getUsername(), StateLog::BUILD_HOUSE, "Bangun rumah di " + name + " (" + code + ") jadi level " + std::to_string(currentLevel));
    }

    return true;
}

void Street::runTile(Player* player) {
    if (player == nullptr) {
        return;
    }
    
    Logger &logger = Logger::getInstance();
    GameManager& gm = GameManager::getInstance();


    // buy mechanism
    if (propertyStatus == BANK) {
        int price = landCost;
        if (player->getDiscount() > 0.0f) {
            price = landCost - static_cast<int>(landCost * player->getDiscount() / 100.0f);
        }

        gm.writeLine("Kamu mendarat di " + name + " (" + code + ")!");
        gm.writeLine("+================================+");
        gm.writeLine("| [" + color + "] " + name + " (" + code + ")   ");
        gm.writeLine("| Harga Beli    : M" + to_string(price));
        int baseRent = (rentCost.size() > 0) ? rentCost[0] : 0;
        gm.writeLine("| Sewa dasar    : M" + to_string(baseRent));
        gm.writeLine("| ...                            |");
        gm.writeLine("+================================+");
        gm.writeLine("Uang kamu saat ini: M" + to_string(player->getCurrency()));
        
        bool wantBuy = false;
        
        //premature check currency because the spec says so
        if (player->getCurrency() >= price) {
            const std::string choice = gm.getCommandHandler().askChoice(
                "Apakah kamu ingin membeli properti ini seharga M" + std::to_string(price) + "? (y/n): ",
                {"y", "n"}
            );

            wantBuy = (choice == "y");
        }
        
        if (wantBuy) {
            *player -= price;
            setOwner(player);
            setPropertyStatus(OWNED);
            gm.writeLine(name + " kini menjadi milikmu!");
            gm.writeLine("Uang tersisa: M" + to_string(player->getCurrency()));
            logger.log(player->getUsername(), StateLog::BUY_TILE, "Beli " + name + " (" + code + ") seharga " + to_string(price));
        } else {
            gm.writeLine("Properti ini akan masuk ke sistem lelang...");
            gm.auction(this);
        }
        return;
    }

    if (propertyStatus == MORTGAGED && owner != nullptr && owner != player) {
        gm.writeLine("Kamu mendarat di " + name + " (" + code + "), milik " + owner->getUsername() + ".");
        gm.writeLine("Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.");
        return;
    }

    // rent mechanism
    if (propertyStatus == OWNED && owner != nullptr && owner != player) {
        gm.writeLine("Kamu mendarat di " + name + " (" + code + "), milik " + owner->getUsername() + "!");
        gm.writeLine("");

        int rent = getRentCost();
        string houseCount;
        string mulLog;

        if (currentLevel < 5 && currentLevel > 0) {
            houseCount = to_string(currentLevel) + " rumah";
        } else if (currentLevel == 5) {
            houseCount = "Hotel";
        } else {
            houseCount = "Tanah kosong";
        }

        if (festivalMultiplier != 1) {
            mulLog = ", festival aktif x" + to_string(festivalMultiplier);
        }

        gm.writeLine("Kondisi      : " + houseCount + mulLog);
        gm.writeLine("Sewa         : M" + to_string(rent));
        gm.writeLine("");

        string rentLog = "Bayar " + to_string(rent) + " ke " + owner->getUsername() + " (" + code + ", " + houseCount + mulLog + ")";
        logger.log(player->getUsername(), StateLog::PAY_RENT, rentLog);

        int playerMoneyBefore = player->getCurrency();
        int ownerMoneyBefore = owner->getCurrency();
        bool canPayNormally = playerMoneyBefore >= rent;

        gm.pay(player, rent, owner);

        if (canPayNormally) {
            gm.writeLine("Uang kamu     : M" + to_string(playerMoneyBefore) + " -> M" + to_string(player->getCurrency()));
            gm.writeLine("Uang " + owner->getUsername() + " : M" + to_string(ownerMoneyBefore) + " -> M" + to_string(owner->getCurrency()));
        }
    }
}

int Street::getRentCost() const {
    if (currentLevel >= 0 && currentLevel < (int)rentCost.size()) {
        return rentCost[currentLevel] * festivalMultiplier;
    }
    return 0;
}

int Street::getBuildingValue() const {
    int level = this->getCurrentLevel();
    int buildingVal = 0;    
    if (level >= 1 && level <= 4) {
        buildingVal = level * this->getHouseCost();
    } else if (level == 5) {
        buildingVal = (4 * this->getHouseCost()) + this->getHotelCost();
    }
    return buildingVal;
}

int Street::getAssetValue() const {
    int buildingValue = 0;

    if (currentLevel >= 1 && currentLevel <= 4) {
        buildingValue = currentLevel * houseCost;
    } else if (currentLevel == 5) {
        buildingValue = 4 * houseCost + hotelCost;
    }

    return landCost + buildingValue;
}
