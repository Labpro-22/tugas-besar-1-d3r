#include "../../include/core/AuctionManager.hpp"

#include <algorithm>
#include <sstream>
#include <string>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

using namespace std;

std::vector<Player*> AuctionManager::getAuctionOrder(const std::vector<Player*>& players, Player* triggerPlayer, Player* excludedPlayer) const {
    std::vector<Player*> order;
    if (players.empty()) {
        return order;
    }

    auto it = std::find(players.begin(), players.end(), triggerPlayer);
    size_t startIndex = 0;
    if (it != players.end()) {
        startIndex = (static_cast<size_t>(std::distance(players.begin(), it)) + 1) % players.size();
    }

    for (size_t offset = 0; offset < players.size(); offset++) {
        Player* player = players[(startIndex + offset) % players.size()];
        if (player != nullptr && player != excludedPlayer && player->getStatus() != BANKRUPT) {
            order.push_back(player);
        }
    }

    return order;
}

    void AuctionManager::transferProperty(Property* property, Player* winner, int price) const {
        if (property == nullptr || winner == nullptr) {
            return;
        }

        *winner -= price;
        property->setOwner(winner);
        property->setPropertyStatus(OWNED);
    }

bool AuctionManager::runAuction(Property* property, Player* excludedPlayer) const {
    if (property == nullptr || property->getPropertyStatus() != BANK) {
        return false;
    }

    GameManager& game = GameManager::getInstance();
    std::vector<Player*> players = game.getPlayer();
    Player* triggerPlayer = game.getCurrentTurnPlayer();
    std::vector<Player*> order = getAuctionOrder(players, triggerPlayer, excludedPlayer);
    if (order.empty()) {
        return false;
    }

    if (order.size() == 1) {
        transferProperty(property, order.front(), 0);
        game.writeLine("Winner (only 1) - " + order.front()->getUsername());
        return true;
    }

    Player* highestBidder = nullptr;
    int highestBid = -1;
    int consecutivePasses = 0;
    const int requiredPasses = static_cast<int>(order.size()) - 1;
    size_t currentIndex = 0;

    while (highestBidder == nullptr || consecutivePasses < requiredPasses) {
        Player* currentPlayer = order[currentIndex];
        game.writeLine("BID Turn: " + currentPlayer->getUsername());

        const std::string line = game.getCommandHandler().askInput("Masukkan PASS atau BID <angka>: ");
        if (line.empty()) {
            return false;
        }

        std::istringstream iss(line);
        std::string action;
        iss >> action;

        if (action == "PASS") {
            // when there is already a bidder, poker like
            if (highestBidder == nullptr && consecutivePasses + 1 >= requiredPasses) {
                game.writeLine("Must bid");
                continue;
            }

            consecutivePasses++;
            currentIndex = (currentIndex + 1) % order.size();
            continue;
        }

        // BID int 
        if (action == "BID") {
            int amount;
            if (!(iss >> amount)) {
                continue;
            }
            if (amount < 0) {
                continue;
            }
            if (highestBidder != nullptr && amount <= highestBid) {
                continue;
            }
            if (amount > currentPlayer->getCurrency()) {
                continue;
            }

            highestBidder = currentPlayer;
            highestBid = amount;
            consecutivePasses = 0;
            currentIndex = (currentIndex + 1) % order.size();
            continue;
        }
    }

    transferProperty(property, highestBidder, highestBid);

    game.writeLine("Winner - " + highestBidder->getUsername() + " price: " + std::to_string(highestBid));

    return true;
}
