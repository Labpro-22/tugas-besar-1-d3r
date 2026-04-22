#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/GameManager.hpp"

PPH::PPH(int index, const std::string& code, const std::string& color, int flatTax, int taxPercentage): Tax(index, code, color), flatTax(flatTax), taxPercentage(taxPercentage) {}
void PPH::payTax(Player* player) {
    if (player != nullptr) {
        // unfinished but in here we ask another input, if user input 1 then
        // payPphTax(player, FLAT)
        // if user input 2 then
        // payPphTax(player, PRECENTAGE)

    }

}

void PPH::payPphTax(Player* player, PPH_OPTION option) {
    int taxAmount = 0;
    if (option == FLAT){
        taxAmount = calculateFlatTax();
    } else {
        taxAmount = calculatePercentageTax(*player);
    }
    *player -= taxAmount;
}
int PPH::calculateFlatTax() const {
    return flatTax;
}
int PPH::calculatePercentageTax(const Player& player) const {
    int total = player.getTotalWealth(&GameManager::getInstance().getBoard());
    int taxAmount = total * taxPercentage/100;
    return taxAmount;
}

PBM::PBM(int index, const std::string& code, const std::string& color, int fixedTax): Tax(index, code, color), fixedTax(fixedTax) {}

int PBM::getFixedTax() const {
    return fixedTax;
}
void PBM::payTax(Player* player) {
    if (player != nullptr) {
        *player -= fixedTax;
    }
}
