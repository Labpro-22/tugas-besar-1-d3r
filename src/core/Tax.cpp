#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/GameManager.hpp"
#include "../../include/core/Logger.hpp"

PPH::PPH(int index, const std::string& code, const std::string& color, int flatTax, int taxPercentage): Tax(index, code, "Pajak Penghasilan", color), flatTax(flatTax), taxPercentage(taxPercentage) {}
void PPH::payTax(Player* player) {
    if (player != nullptr) {
        CommandHandler& handler = GameManager::getInstance().getCommandHandler();
        GameManager::getInstance().writeLine("Pilih metode pembayaran:");
        GameManager::getInstance().writeLine("1. Flat");
        GameManager::getInstance().writeLine("2. Persentase");

        const int choice = handler.askInt("Pilihan (1-2): ", 1, 2);
        if (choice == 1) {
            payPphTax(player, FLAT);
        } else {
            payPphTax(player, PERCENTAGE);
        }
    }
}

void PPH::payPphTax(Player* player, PPH_OPTION option) {
    Logger &logger = Logger::getInstance();
    int taxAmount = 0;
    if (option == FLAT){
        taxAmount = calculateFlatTax();
        logger.log(player->getUsername(), StateLog::PAY_TAX, 
                    "Pemain membayar pajak PPH tipe FLAT sebesar " + to_string(taxAmount));
    } else {
        taxAmount = calculatePercentageTax(*player);
        logger.log(player->getUsername(), StateLog::PAY_TAX, 
                    "Pemain membayar pajak PPH tipe PERSENTASE sebesar " + to_string(taxAmount));
    }
    GameManager::getInstance().pay(player, taxAmount, nullptr);
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
    Logger &logger = Logger::getInstance();
    if (player != nullptr) {
        logger.log(player->getUsername(), StateLog::PAY_TAX, 
            "Pemain membayar pajak PBM sebesar " + to_string(fixedTax));
        GameManager::getInstance().pay(player, fixedTax, nullptr);
    }
}
