#include "../../include/core/AuctionManager.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

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

    Player* previousOwner = property->getOwner();
    if (previousOwner != nullptr && previousOwner != winner) {
        std::vector<Property*> previousProperties = previousOwner->getOwnedProperties();
        previousProperties.erase(std::remove(previousProperties.begin(), previousProperties.end(), property), previousProperties.end());
        previousOwner->setOwnedProperties(previousProperties);
    }

    std::vector<Property*> winnerProperties = winner->getOwnedProperties();
    if (std::find(winnerProperties.begin(), winnerProperties.end(), property) == winnerProperties.end()) {
        winnerProperties.push_back(property);
        winner->setOwnedProperties(winnerProperties);
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
        std::cout << "Winner (only 1) - " << order.front()->getUsername() << std::endl;
        return true;
    }

    Player* highestBidder = nullptr;
    int highestBid = -1;
    int consecutivePasses = 0;
    const int requiredPasses = static_cast<int>(order.size()) - 1;
    size_t currentIndex = 0;

    while (highestBidder == nullptr || consecutivePasses < requiredPasses) {
        Player* currentPlayer = order[currentIndex];
        std::cout << "BID Turn: " << currentPlayer->getUsername() << std::endl;

        std::string line;
        if (!std::getline(std::cin, line)) {
            return false;
        }

        std::istringstream iss(line);
        std::string action;
        iss >> action;

        if (action == "PASS") {
            // when there is already a bidder, poker like
            if (highestBidder == nullptr && consecutivePasses + 1 >= requiredPasses) {
                std::cout << "Must bid" << std::endl;
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

    std::cout << "Winner - " << highestBidder->getUsername() << " price: " << highestBid << std::endl;

    return true;
}
