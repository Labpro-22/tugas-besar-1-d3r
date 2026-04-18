#include "../../include/core/Tile.hpp"


Railroad::Railroad(int index, const std::string& code, const std::string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus, const std::vector<int>& rentCost) : Property(index, code, color, landCost, mortgageValue, festivalMultiplier, festivalDuration, owner, propertyStatus), rentCost(rentCost) {}

// placeholder for now
// void Railroad::runTile(Player*) {

// }
// int Railroad::getRentCost(Player*) {
//     return 0;
// }