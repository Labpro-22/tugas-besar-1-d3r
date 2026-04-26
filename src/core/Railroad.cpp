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
        std::string condition = std::to_string(level) + " stasiun dimiliki";
        if (festivalMultiplier != 1) {
            condition += ", festival aktif x" + std::to_string(festivalMultiplier);
        }

        gm.writeLine("Kondisi      : " + condition);
        gm.writeLine("Sewa         : M" + std::to_string(rent));
        gm.writeLine("");

        std::string rentLog = "Bayar " + std::to_string(rent) + " ke " + owner->getUsername() +
                            " (" + code + ", level " + std::to_string(level) + ")";
        logger.log(player->getUsername(), StateLog::PAY_RENT, rentLog);

        int playerMoneyBefore = player->getCurrency();
        int ownerMoneyBefore = owner->getCurrency();
        bool canPayNormally = playerMoneyBefore >= rent;

        gm.pay(player, rent, owner);

        if (canPayNormally) {
            std::string playerMoneyLabel = "Uang kamu";
            if (playerMoneyLabel.length() < 14) {
                playerMoneyLabel += std::string(14 - playerMoneyLabel.length(), ' ');
            }

            std::string ownerMoneyLabel = "Uang " + owner->getUsername();
            if (ownerMoneyLabel.length() < 14) {
                ownerMoneyLabel += std::string(14 - ownerMoneyLabel.length(), ' ');
            }

            gm.writeLine(playerMoneyLabel + ": M" + std::to_string(playerMoneyBefore) +
                        " -> M" + std::to_string(player->getCurrency()));
            gm.writeLine(ownerMoneyLabel + ": M" + std::to_string(ownerMoneyBefore) +
                        " -> M" + std::to_string(owner->getCurrency()));
        }
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
