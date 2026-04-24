#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

Railroad::Railroad(int index, const std::string &code, const std::string &name,
                   const std::string &color, int landCost,
                   int mortgageValue, int festivalMultiplier,
                   int festivalDuration, Player *owner,
                   PROPERTY_STATUS propertyStatus,
                   const std::vector<int> &rentCost) : Property(index, code, name, color, landCost,
                                                                mortgageValue, festivalMultiplier,
                                                                festivalDuration, owner, propertyStatus),
                                                       rentCost(rentCost) {}

void Railroad::runTile(Player *player){
    Logger &logger = Logger::getInstance();
    GameManager& gm = GameManager::getInstance();
    if (player == nullptr) {
        return;
    }

    // buy mechanism
    if (propertyStatus == BANK) {
        gm.writeLine("Kamu mendarat di " + name + " (" + code + ")!");
        if (player->getCurrency() >= landCost) {
            *player -= landCost;
            this->setOwner(player);
            this->setPropertyStatus(OWNED);
            logger.log(player->getUsername(), StateLog::RAILROAD, code + " kini milik " + player->getUsername() + " (otomatis)");
            gm.writeLine("Belum ada yang menginjaknya duluan, stasiun ini kini menjadi milikmu!");
        } else {
            gm.writeLine("Kamu tidak memiliki cukup uang untuk mengamankan stasiun ini.");
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

    // rent mechanism
    if (propertyStatus == OWNED && owner != nullptr && owner != player) {
        int rent = getRentCost();
        
        gm.writeLine("Kamu mendarat di " + name + " (" + code + "), milik " + owner->getUsername() + "!");
        gm.writeLine("");
        
        int level = gm.getBoard().getRailroadLevel(const_cast<Railroad *>(this));
        gm.writeLine("Kondisi      : " + to_string(level) + " stasiun dimiliki");
        gm.writeLine("Sewa         : M" + to_string(rent));
        gm.writeLine("");
        
        std::string mulLog;
        if(festivalMultiplier != 1) mulLog = ", festival aktif x" + to_string(festivalMultiplier);
        std::string rentLog = "Bayar " + to_string(rent) + " ke " + owner->getUsername() + " (" + code + ", level" + to_string(level) + mulLog + ")";
        logger.log(player->getUsername(), StateLog::PAY_RENT, rentLog);
        
        gm.pay(player, rent, owner);
    }
}

int Railroad::getRentCost() const
{
    if (owner == nullptr || rentCost.empty()) {
        return 0;
    }
    int level = GameManager::getInstance().getBoard().getRailroadLevel(const_cast<Railroad *>(this));
    if (level <= 0) {
        return 0;
    }

    if (level > static_cast<int>(rentCost.size())) {
        level = static_cast<int>(rentCost.size());
    }

    int cost = rentCost[level - 1] * festivalMultiplier;
    return cost;
}
