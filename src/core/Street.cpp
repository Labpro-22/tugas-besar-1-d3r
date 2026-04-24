#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"

#include <algorithm>

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

    if (propertyStatus == BANK) {
        int price = landCost;
        if (player->getDiscount() > 0.0f) {
            price = landCost - static_cast<int>(landCost * player->getDiscount() / 100.0f);
        }

        if (player->getCurrency() >= price) {
            *player -= price;
            owner = player;
            propertyStatus = OWNED;
            std::vector<Property*> properties = player->getOwnedProperties();
            if (std::find(properties.begin(), properties.end(), this) == properties.end()) {
                properties.push_back(this);
                player->setOwnedProperties(properties);
            }
        }
        return;
    }

    if (propertyStatus == OWNED && owner != nullptr && owner != player) {
        int rent = getRentCost();
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
