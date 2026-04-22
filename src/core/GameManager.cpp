#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

std::vector<int> GameManager::dice;

GameManager::GameManager() 
    : turn(0), maxTurn(0), activePlayerCount(0), playerCount(0), currentTurnPlayer(nullptr), board(40) {
    // Board initialized with 40 tiles (standard Monopoly)
}

bool GameManager::isGameValid() {
    return playerCount > 0 && maxTurn > 0;
}

void GameManager::runGame() {
    // TODO: Implement game loop
}

void GameManager::auction(Tile*) {
    // TODO: Implement auction logic
}

void GameManager::initBoard() {
    // TODO: Initialize board with tiles
}

void GameManager::initPlayers() {
    // TODO: Initialize players
}

void GameManager::initStateLogs() {
    // TODO: Initialize state logs
}

void GameManager::initSkillDeck() {
    // TODO: Initialize skill deck
}

Logger& GameManager::getLogger() {
    return logger;
}
