#include "../../include/core/GameManager.hpp"

#include <algorithm>
#include <iostream>
#include <string>

#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

GameManager::GameManager() 
    : turn(0), maxTurn(0), activePlayerCount(0), playerCount(0), initialCurrency(0), board(40), currentTurnPlayer(nullptr) {
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
    if (players.empty()) {
        CommandHandler& handler = getCommandHandler();
        const int count = handler.askInt("Jumlah pemain (2-4): ", 2, 4);

        std::vector<Player*> newPlayers;
        for (int i = 0; i < count; i++) {
            const std::string username = handler.askInput("Username pemain " + std::to_string(i + 1) + ": ");

            Player* player = new Player();
            player->setUsername(username);
            player->setCurrency(initialCurrency);
            newPlayers.push_back(player);
        }

        setPlayers(newPlayers);
        initPlayers();
        initSkillDeck();
        initAutoUseDecks();
        drawSkillCard(currentTurnPlayer);
    }

    if (!isGameValid()) {
        std::cout << "[WARN] Game state is not valid yet." << std::endl;
        return;
    }

    std::cout << "[INFO] Game ready." << std::endl;
}

void GameManager::auction(Tile* tile) {
    Property* property = dynamic_cast<Property*>(tile);
    if (property == nullptr) {
        return;
    }

    auctionManager.runAuction(property);
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

void GameManager::rollDice() {
    dice.roll();
    rollDice(dice.getFirst(), dice.getSecond());
}

void GameManager::rollDice(int dice1, int dice2) {
    if (currentTurnPlayer == nullptr || currentTurnPlayer->getCurrentTile() == nullptr) {
        return;
    }

    if (!dice.setValues(dice1, dice2)) {
        std::cout << "Nilai dadu harus 1 sampai 6." << std::endl;
        return;
    }

    int total = dice.getTotal();
    Tile* destination = board.goToTile(*currentTurnPlayer->getCurrentTile(), total);

    std::cout << "Hasil: " << dice.getFirst() << " + " << dice.getSecond() << " = " << total << std::endl;
    if (destination == nullptr) {
        std::cout << "Tujuan tidak valid." << std::endl;
        return;
    }

    currentTurnPlayer->moveTo(destination, true);
    std::cout << "Mendarat di: " << destination->getName() << " (" << destination->getCode() << ")" << std::endl;

    if (!dice.isDouble()) {
        nextTurn();
    } else {
        std::cout << "Double. Pemain mendapat giliran tambahan." << std::endl;
    }
}

void GameManager::initAutoUseDecks() {
    deckChance.addCard(new NearestStationCard());
    deckChance.addCard(new MoveBackCard());
    deckChance.addCard(new ToJailCard());
    deckChance.shuffleDeck();

    deckCurrency.addCard(new BirthDayCard());
    deckCurrency.addCard(new DoctorCard());
    deckCurrency.addCard(new CampaignCard());
    deckCurrency.shuffleDeck();
}

void GameManager::drawSkillCard(Player* player) {
    if (player == nullptr) {
        return;
    }

    SkillCard* card = deckSkill.getRandomCard();
    if (card == nullptr) {
        return;
    }

    if (!player->addSkillCard(card)) {
        delete card;
    }
}

void GameManager::nextTurn() {
    if (players.empty()) {
        currentTurnPlayer = nullptr;
        return;
    }

    if (currentTurnPlayer != nullptr) {
        currentTurnPlayer->endTurnEffects();
    }

    auto it = std::find(players.begin(), players.end(), currentTurnPlayer);
    size_t nextIndex = 0;
    if (it != players.end()) {
        nextIndex = (static_cast<size_t>(std::distance(players.begin(), it)) + 1) % players.size();
        if (nextIndex == 0) {
            turn++;
        }
    }

    currentTurnPlayer = players[nextIndex];
    if (currentTurnPlayer != nullptr) {
        currentTurnPlayer->resetCardUse();
        drawSkillCard(currentTurnPlayer);
        // drawSkillCard(currentTurnPlayer);
        // drawSkillCard(currentTurnPlayer);
        // drawSkillCard(currentTurnPlayer);

    }
}

bool GameManager::isGameFinished() const {
    int activeCount = 0;
    for (Player* player : players) {
        if (player != nullptr && player->getStatus() != BANKRUPT) {
            activeCount++;
        }
    }

    if (!players.empty() && activeCount <= 1) {
        return true;
    }

    return maxTurn > 0 && turn >= maxTurn;
}

Player* GameManager::getWinner() const {
    Player* winner = nullptr;
    for (Player* player : players) {
        if (player == nullptr || player->getStatus() == BANKRUPT) {
            continue;
        }

        if (winner == nullptr || player->getCurrency() > winner->getCurrency()) {
            winner = player;
        }
    }

    return winner;
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
