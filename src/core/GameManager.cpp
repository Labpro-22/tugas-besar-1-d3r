#include "../../include/core/GameManager.hpp"

#include <algorithm>
#include <iostream>

#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

std::vector<int> GameManager::dice;

GameManager::GameManager() 
    : turn(0), maxTurn(0), activePlayerCount(0), playerCount(0), board(40), currentTurnPlayer(nullptr) {
    // Board initialized with 40 tiles (standard Monopoly)
}

bool GameManager::isGameValid() {
    if (maxTurn <= 0) {
        return false;
    }

    if (playerCount < 0 || activePlayerCount < 0) {
        return false;
    }

    if (playerCount != static_cast<int>(players.size())) {
        return false;
    }

    return std::all_of(players.begin(), players.end(), [](Player* player) {
        return player != nullptr;
    });
}

void GameManager::runGame() {
    // TODO: Implement full game loop
    if (!isGameValid()) {
        std::cout << "[WARN] Game state is not valid yet." << std::endl;
        return;
    }

    std::cout << "[INFO] Game ready. Turn " << turn << " / " << maxTurn << std::endl;
}

void GameManager::auction(Tile* tile) {
    // TODO: Implement auction logic
    Property* property = dynamic_cast<Property*>(tile);
    if (property == nullptr || property->getPropertyStatus() != BANK) {
        return;
    }

    std::cout << "[INFO] Auction requested for " << property->getCode() << std::endl;
}

void GameManager::initBoard() {
    board = Board(40);
}

void GameManager::initPlayers() {
    playerCount = static_cast<int>(players.size());
    activePlayerCount = 0;

    Tile* startTile = board.getTile("GO");
    for (Player* player : players) {
        if (player == nullptr) {
            continue;
        }

        player->setCurrentStatus(ACTIVE);
        player->setCurrentTile(startTile);
        activePlayerCount++;
    }

    currentTurnPlayer = players.empty() ? nullptr : players.front();
}

void GameManager::initStateLogs() {
    // TODO: Initialize state logs
}

void GameManager::initSkillDeck() {
    for (int i = 0; i < 4; i++) {
        deckSkill.addCard(new MoveCard());
    }

    for (int i = 0; i < 3; i++) {
        deckSkill.addCard(new DiscountCard());
    }

    for (int i = 0; i < 2; i++) {
        deckSkill.addCard(new ShieldCard());
        deckSkill.addCard(new TeleportCard());
        deckSkill.addCard(new LassoCard());
        deckSkill.addCard(new DemolitionCard());
    }

    deckSkill.shuffleDeck();
}

Logger& GameManager::getLogger() {
    return logger;
}
