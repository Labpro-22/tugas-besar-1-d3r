#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"

Utility::Utility(int index, const std::string& code, const std::string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus, const std::vector<int>& costMultiplier) : Property(index, code, color, landCost, mortgageValue, festivalMultiplier, festivalDuration, owner, propertyStatus), costMultiplier(costMultiplier) {}

void Utility::runTile(Player* player) {
    if (player != nullptr) {
        // TODO: Implement utility tile logic (collect rent, etc)
    }
}

int Utility::getRentCost() const {
    if (!costMultiplier.empty()) {
        return costMultiplier[0];
    }
    return 0;
}