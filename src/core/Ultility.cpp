#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/GameManager.hpp"

Utility::Utility(int index, const std::string& code, const std::string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus, const std::vector<int>& costMultiplier) : Property(index, code, color, landCost, mortgageValue, festivalMultiplier, festivalDuration, owner, propertyStatus), costMultiplier(costMultiplier) {}

void Utility::runTile(Player* player) {
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

int Utility::getRentCost() const {
    if (owner == nullptr || costMultiplier.empty()){
        return 0;
    }

    int level = GameManager::getInstance().getBoard().getUtilityLevel(const_cast<Utility*>(this));
    if (level <= 0){
        return 0;
    }

    if (level > static_cast<int>(costMultiplier.size())){
        level = static_cast<int>(costMultiplier.size());
    }

    // calculate the dice val
    vector<int> dice = GameManager::getDice();
    int totalValDice = 0;
    for (int val : dice){
        totalValDice += val;
    }

    int cost = totalValDice * costMultiplier[level-1]*festivalMultiplier;
    return cost;
}