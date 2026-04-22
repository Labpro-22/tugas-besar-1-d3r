#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"

PPH::PPH(int index, const std::string& code, const std::string& color, int flatTax, int taxPercentage): Tax(index, code, color), flatTax(flatTax), taxPercentage(taxPercentage) {}
void PPH::payTax(Player* player) {
    if (player != nullptr) {
        int taxAmount = flatTax + (player->getCurrency() * taxPercentage / 100);
        *player -= taxAmount;
    }
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
