#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

#include <algorithm>

Railroad::Railroad(int index, const std::string &code, const std::string &name,
                   const std::string &color, int landCost,
                   int mortgageValue, int festivalMultiplier,
                   int festivalDuration, Player *owner,
                   PROPERTY_STATUS propertyStatus,
                   const std::vector<int> &rentCost) : Property(index, code, name, color, landCost,
                                                                mortgageValue, festivalMultiplier,
                                                                festivalDuration, owner, propertyStatus),
                                                       rentCost(rentCost) {}

void Railroad::runTile(Player *player)
{
    Logger logger = Logger::getInstance();
    if (player == nullptr) {
        return;
    }

    if (propertyStatus == BANK) {
        if (player->getCurrency() >= landCost) {
            *player -= landCost;
            owner = player;
            propertyStatus = OWNED;
            std::vector<Property *> properties = player->getOwnedProperties();
            if (std::find(properties.begin(), properties.end(), this) == properties.end()) {
                properties.push_back(this);
                player->setOwnedProperties(properties);
            }
            logger.log(player->getUsername(), StateLog::RAILROAD, code + " kini milik " + player->getUsername() + " (otomatis)");
        }
        return;
    }

    if (propertyStatus == OWNED && owner != nullptr && owner != player) {
        int rent = getRentCost();
        std::string mulLog;
        if(festivalMultiplier != 1) mulLog = ", festival aktif x" + to_string(festivalMultiplier);
        int level = GameManager::getInstance().getBoard().getRailroadLevel(const_cast<Railroad *>(this));
        std::string rentLog = "Bayar " + to_string(rent) + " ke " + owner->getUsername() + " (" + code + ", level" + to_string(level) + mulLog + ")";
        logger.log(player->getUsername(), StateLog::PAY_RENT, rentLog);
        *player -= rent;
        *owner += rent;
    }
}

int Railroad::getRentCost() const
{
    if (owner == nullptr || rentCost.empty()) {
        return 0;
    }
    int level = GameManager::getInstance().getBoard().getRailroadLevel(const_cast<Railroad *>(this));
    if (level <= 0) {
        return 0;
    }

    if (level > static_cast<int>(rentCost.size())) {
        level = static_cast<int>(rentCost.size());
    }

    int cost = rentCost[level - 1] * festivalMultiplier;
    return cost;
}
