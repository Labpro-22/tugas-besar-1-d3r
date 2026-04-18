#include "../../include/core/Tile.hpp"

PPH::PPH(int index, const std::string& code, const std::string& color) : Tax(index, code, color) {}
void PPH::payTax(Player *) {

}

PBM::PBM(int index, const std::string& code, const std::string& color, int fixedTax) :  Tax(index, code, color), fixedTax(fixedTax) {}

int PBM::getFixedTax() const {
    return fixedTax;
}
void PPH::payTax(Player *) {

}
