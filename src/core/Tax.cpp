#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"

PPH::PPH(int index, const std::string& code, const std::string& color,const std::string& name, int flatTax, int taxPercentage): Tax(index, code, color,name), flatTax(flatTax), taxPercentage(taxPercentage) {}
void PPH::payTax(Player* player) {
    if (player != nullptr) {
        int taxAmount = flatTax + (player->getCurrency() * taxPercentage / 100);
        *player -= taxAmount;
    }
}

PBM::PBM(int index, const std::string& code, const std::string& color,const std::string& name, int fixedTax): Tax(index, code, color, name), fixedTax(fixedTax) {}

int PBM::getFixedTax() const {
    return fixedTax;
}
void PBM::payTax(Player* player) {
    if (player != nullptr) {
        *player -= fixedTax;
    }
}
