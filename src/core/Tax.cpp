#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/GameManager.hpp"

#include <iostream>

PPH::PPH(int index, const std::string& code, const std::string& color, int flatTax, int taxPercentage): Tax(index, code, "Pajak Penghasilan", color), flatTax(flatTax), taxPercentage(taxPercentage) {}
void PPH::payTax(Player* player) {
    if (player != nullptr) {
        CommandHandler& handler = GameManager::getInstance().getCommandHandler();
        const int flat = calculateFlatTax();
        const int percentage = calculatePercentageTax(*player);

        std::cout << "Pilih metode pembayaran:\n";
        std::cout << "1. Flat\n";
        std::cout << "2. Persentase\n";

        const int choice = handler.askInt("Pilihan (1-2): ", 1, 2);
        if (choice == 1) {
            payPphTax(player, FLAT);
        } else {
            payPphTax(player, PERCENTAGE);
        }
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

PBM::PBM(int index, const std::string& code, const std::string& name, const std::string& color, int fixedTax): Tax(index, code, name, color), fixedTax(fixedTax) {}

int PBM::getFixedTax() const {
    return fixedTax;
}
void PBM::payTax(Player* player) {
    if (player != nullptr) {
        *player -= fixedTax;
    }
}
