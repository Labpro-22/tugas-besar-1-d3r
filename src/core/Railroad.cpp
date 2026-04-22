#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/GameManager.hpp"

Railroad::Railroad(int index, const std::string& code, const std::string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus, const std::vector<int>& rentCost) : Property(index, code, color, landCost, mortgageValue, festivalMultiplier, festivalDuration, owner, propertyStatus), rentCost(rentCost) {}

void Railroad::runTile(Player* player) {
    if (player == nullptr) {
        return;
    }

    if (propertyStatus == BANK) {
    
        owner = player;
        propertyStatus = OWNED;
        return;
    }

    if (propertyStatus == OWNED && owner != nullptr && owner != player) {
        int rent = getRentCost();
        *player -= rent;
        *owner += rent;
    }
}

int Railroad::getRentCost() const {
    if (owner == nullptr || rentCost.empty()){
        return 0;
    }
    int level = GameManager::getInstance().getBoard().getRailroadLevel(const_cast<Railroad*>(this));
    if (level <= 0){
        return 0;
    }

    if (level > static_cast<int>(rentCost.size())){
        level = static_cast<int>(rentCost.size());
    }

    int cost = rentCost[level-1]*festivalMultiplier;
    return cost;
}
