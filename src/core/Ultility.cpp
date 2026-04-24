#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/GameManager.hpp"

using namespace std;

Utility::Utility(int index, const string& code, const string& name, const string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus, const vector<int>& costMultiplier) : Property(index, code, name, color, landCost, mortgageValue, festivalMultiplier, festivalDuration, owner, propertyStatus), costMultiplier(costMultiplier) {}

void Utility::runTile(Player *player) {
    Logger &logger = Logger::getInstance();
    GameManager& gm = GameManager::getInstance();
    if (player == nullptr) {
        return;
    }

    if (propertyStatus == BANK) {
        gm.writeLine("Kamu mendarat di " + name + " (" + code + ")!");
        if (player->getCurrency() >= landCost) {
            *player -= landCost;
            setOwner(player);
            setPropertyStatus(OWNED);
            logger.log(player->getUsername(), StateLog::UTILITY, code + " kini milik " + player->getUsername() + " (otomatis)");
            gm.writeLine("Belum ada yang menginjaknya duluan, utilitas ini kini menjadi milikmu!");
        } else {
            gm.writeLine("Kamu tidak memiliki cukup uang untuk mengamankan utilitas ini.");
            gm.writeLine("Properti ini akan masuk ke sistem lelang...");
            gm.auction(this);
        }
        return;
    }

    if (propertyStatus == MORTGAGED && owner != nullptr && owner != player) {
        gm.writeLine("Kamu mendarat di " + name + " (" + code + "), milik " + owner->getUsername() + ".");
        gm.writeLine("Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.");
        return;
    }

    if (propertyStatus == OWNED && owner != nullptr && owner != player) {
        int rent = getRentCost();
        
        gm.writeLine("Kamu mendarat di " + name + " (" + code + "), milik " + owner->getUsername() + "!");
        gm.writeLine("");
        
        int level = gm.getBoard().getUtilityLevel(const_cast<Utility *>(this));
        int totalDice = gm.getDice().getTotal();
        gm.writeLine("Kondisi      : " + to_string(level) + " utilitas dimiliki");
        gm.writeLine("Perhitungan  : Total Dadu (" + to_string(totalDice) + ") * Pengali ");
        gm.writeLine("Sewa         : M" + to_string(rent));
        gm.writeLine("");
        
        string mulLog;
        if(festivalMultiplier != 1) mulLog = ", festival aktif x" + to_string(festivalMultiplier);
        string rentLog = "Bayar " + to_string(rent) + " ke " + owner->getUsername() + " (" + code + ", level" + to_string(level) + mulLog + ")";
        logger.log(player->getUsername(), StateLog::PAY_RENT, rentLog);
        
        gm.pay(player, rent, owner);
    }
}

int Utility::getRentCost() const {
    if (owner == nullptr || costMultiplier.empty()) {
        return 0;
    }

    int level = GameManager::getInstance().getBoard().getUtilityLevel(const_cast<Utility*>(this));
    if (level <= 0) {
        return 0;
    }
    if (level > static_cast<int>(costMultiplier.size())) {
        level = static_cast<int>(costMultiplier.size());
    }
    int totalDice = GameManager::getInstance().getDice().getTotal();

    return totalDice * costMultiplier[level - 1] * festivalMultiplier;
}
