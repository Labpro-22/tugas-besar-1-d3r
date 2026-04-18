#include "../../include/core/Tile.hpp"


Utility::Utility(int index, const std::string& code, const std::string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus, const std::vector<int>& costMultiplier) : Property(index, code, color, landCost, mortgageValue, festivalMultiplier, festivalDuration, owner, propertyStatus), costMultiplier(costMultiplier) {}

// placeholder for now
// int Ultility::getRentCost(Player*) {
//     return 0;
// }