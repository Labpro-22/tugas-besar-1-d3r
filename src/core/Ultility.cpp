#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/GameManager.hpp"

#include <algorithm>

Utility::Utility(int index, const std::string& code, const std::string& name, const std::string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus, const std::vector<int>& costMultiplier) : Property(index, code, name, color, landCost, mortgageValue, festivalMultiplier, festivalDuration, owner, propertyStatus), costMultiplier(costMultiplier) {}

void Utility::runTile(Player* player) {
    Logger logger = Logger::getInstance();
    if (player == nullptr) {
        return;
    }

    if (propertyStatus == BANK) {
        if (player->getCurrency() >= landCost) {
            *player -= landCost;
            owner = player;
            propertyStatus = OWNED;
            std::vector<Property*> properties = player->getOwnedProperties();
            if (std::find(properties.begin(), properties.end(), this) == properties.end()) {
                properties.push_back(this);
                player->setOwnedProperties(properties);
            }
            logger.log(player->getUsername(), StateLog::UTILITY, code + " kini milik " + player->getUsername() + " (otomatis)");
        }
        return;
    }

    if (propertyStatus == OWNED && owner != nullptr && owner != player) {
        int rent = getRentCost();
        *player -= rent;
        *owner += rent;
    }
}

int Utility::getRentCost() const {
    if (owner == nullptr || costMultiplier.empty()) {
        return 0;
    }

    int level = GameManager::getInstance().getBoard().getUtilityLevel(const_cast<Utility*>(this));
    if (level <= 0) {
        return 0;
    }
    if (level > static_cast<int>(costMultiplier.size())) {
        level = static_cast<int>(costMultiplier.size());
    }
    int totalDice = GameManager::getInstance().getDice().getTotal();

    return totalDice * costMultiplier[level - 1] * festivalMultiplier;
}
