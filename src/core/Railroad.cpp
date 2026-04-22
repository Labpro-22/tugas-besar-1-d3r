#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"

Railroad::Railroad(int index, const std::string& code, const std::string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus, const std::vector<int>& rentCost) : Property(index, code, color, landCost, mortgageValue, festivalMultiplier, festivalDuration, owner, propertyStatus), rentCost(rentCost) {}

void Railroad::runTile(Player* player) {
    if (player != nullptr) {
        // TODO: Implement railroad tile logic (collect rent, etc)
    }
}

int Railroad::getRentCost() const {
    if (!rentCost.empty()) {
        return rentCost[0];
    }
    return 0;
}