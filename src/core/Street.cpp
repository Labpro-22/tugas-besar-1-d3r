#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Logger.hpp"

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

void Street::runTile(Player* player) {
    if (player == nullptr) {
        return;
    }
    
    Logger &logger = Logger::getInstance();

    if (propertyStatus == BANK) {
        int price = landCost;
        if (player->getDiscount() > 0.0f) {
            price = landCost - static_cast<int>(landCost * player->getDiscount() / 100.0f);
        }

        if (player->getCurrency() >= price) {
            *player -= price;
            setOwner(player);
            setPropertyStatus(OWNED);
            logger.log(player->getUsername(), StateLog::BUY_TILE, "Beli " + name + " (" + code + ") seharga " + to_string(price));
        }
        return;
    }

    if (propertyStatus == OWNED && owner != nullptr && owner != player) {
        int rent = getRentCost();
        std::string houseCount;
        std::string mulLog;
        if(currentLevel < 5 && currentLevel > 0) houseCount = ", " + to_string(currentLevel) + " rumah";
        if(currentLevel == 5) houseCount = ", 4 rumah, 1 hotel";
        if(festivalMultiplier != 1) mulLog = ", festival aktif x" + to_string(festivalMultiplier);
        std::string rentLog = "Bayar " + to_string(rent) + " ke " + owner->getUsername() + " (" + code + houseCount + mulLog + ")";
        logger.log(player->getUsername(), StateLog::PAY_RENT, rentLog);
        *player -= rent;
        *owner += rent;
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
