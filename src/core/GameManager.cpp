#include "../../include/core/GameManager.hpp"

#include <algorithm>
#include <iostream>
#include <string>

#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"
#include "../../include/core/Logger.hpp"

GameManager::GameManager() 
    : turn(0), maxTurn(0), activePlayerCount(0), playerCount(0), initialCurrency(0),
      board(45), currentTurnPlayer(nullptr), useGuiStream(false), doubleCount(0) {
    // Board initialized with 40 tiles (standard Monopoly)
}

void GameManager::write(const std::string& text) const {
    if (useGuiStream && outputCallback) {
        outputCallback(text);
        return;
    }

    std::cout << text;
}

void GameManager::writeLine(const std::string& text) const {
    if (useGuiStream && outputCallback) {
        outputCallback(text);
        return;
    }

    std::cout << text << std::endl;
}

std::string GameManager::readLine(const std::string& prompt) const {
    if (!prompt.empty()) {
        write(prompt);
    }

    if (useGuiStream && inputCallback) {
        return inputCallback(prompt);
    }

    std::string line;
    if (!std::getline(std::cin, line)) {
        return "";
    }

    return line;
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

void GameManager::printTurnInfo() {
    if (currentTurnPlayer == nullptr) {
        return;
    }

    std::string turnInfo = "\n[Turn " + std::to_string(getTurn() + 1);
    if (getMaxTurn() > 0) {
        turnInfo += "/" + std::to_string(getMaxTurn());
    }
    turnInfo += "] " + currentTurnPlayer->getUsername() + " > ";
    write(turnInfo);
}

void GameManager::promptForCommand() {
    if (currentTurnPlayer == nullptr || isGameFinished()) {
        return;
    }

    std::string prompt = "\n[Turn " + std::to_string(getTurn() + 1);
    if (getMaxTurn() > 0) {
        prompt += "/" + std::to_string(getMaxTurn());
    }
    prompt += "] " + currentTurnPlayer->getUsername() + " > ";

    readLine(prompt);
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
        currentTurnPlayer = players[0];
        drawSkillCard(currentTurnPlayer);
    }

    if (!isGameValid()) {
        writeLine("[WARN] Game state is not valid yet.");
        return;
    }

    writeLine("[INFO] Game ready.");
    printTurnInfo();
}

void GameManager::auction(Tile* tile) {
    Property* property = dynamic_cast<Property*>(tile);
    if (property == nullptr) {
        return;
    }

    auctionManager.runAuction(property);
}

void GameManager::initBoard() {
    board = Board(45);
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
        writeLine("Game belum siap. Current player belum diinisialisasi.");
        return;
    }

    if (!dice.setValues(dice1, dice2)) {
        writeLine("Nilai dadu harus 1 sampai 6.");
        return;
    }

    int total = dice.getTotal();
    Tile* destination = board.goToTile(*currentTurnPlayer->getCurrentTile(), total);

    std::string diceResult = "Lempar: " + std::to_string(dice.getFirst()) + " + " + std::to_string(dice.getSecond()) + " = " + std::to_string(total);
    writeLine(diceResult);
    if (destination == nullptr) {
        writeLine("Tujuan tidak valid.");
        return;
    }

    currentTurnPlayer->moveTo(destination, true);
    std::string moveResult = "Mendarat di: " + destination->getName() + " (" + destination->getCode() + ")";
    writeLine(moveResult);
    logger.log(currentTurnPlayer->getUsername(), StateLog::DICE, diceResult + " → " + moveResult);

    if (!dice.isDouble()) {
        nextTurn();
        doubleCount = 0;
    } else {
        doubleCount++;
        writeLine("Double. Pemain mendapat giliran tambahan.");
        logger.log(currentTurnPlayer->getUsername(), StateLog::DOUBLE, "Giliran tambahan ke-" + to_string(doubleCount));
        if(doubleCount == 3){
            currentTurnPlayer->setToJailed();
            currentTurnPlayer->moveTo(board.getJailTile(), false);
        }
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
    }

    // Print turn info for GUI and CLI
    if (currentTurnPlayer != nullptr) {
        std::string turnInfo = "\n[Turn " + std::to_string(getTurn() + 1);
        if (getMaxTurn() > 0) {
            turnInfo += "/" + std::to_string(getMaxTurn());
        }
        turnInfo += "> " + currentTurnPlayer->getUsername() + " turn";
        writeLine(turnInfo);
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

void GameManager::forcePay(Player *debtor, int amount, Player* creditor) {
    if (debtor == nullptr || amount <= 0) {
        return;
    }

    if (debtor->getCurrency() >= amount) {
        *debtor -= amount;
        *creditor += amount;
        return;
    } else {
        handleBankruptcy(debtor, amount, creditor);
    }
}

void GameManager::sellPropertyToBank(Player* player, Property* property) {
    if (player == nullptr || property == nullptr){
        return;
    }

    if (property->getOwner() != player || property->getPropertyStatus() != OWNED){
        return;
    }

    int totSellVal = property->getLandCost();
    Street* street = dynamic_cast<Street*>(property);
    if (street != nullptr) {
        int buildingValue = street->getBuildingValue();
        totSellVal += (buildingValue / 2);
        
        street->setCurrentLevel(0);
    
    }
    
    *player += totSellVal;

    property->setOwner(nullptr);
    property->setPropertyStatus(BANK);
}

void GameManager::handleBankruptcy(Player *debtor, int amount, Player* creditor) {
    GameManager::getInstance().writeLine("");
    string target = creditor ? (" kepada " + creditor->getUsername()) : "";
    string reason = creditor ? "sewa M" : "Pajak/Tagihan M";
    
    GameManager::getInstance().writeLine("Kamu tidak dapat membayar " + reason + to_string(amount) + target + "!");
    GameManager::getInstance().writeLine("");

    GameManager::getInstance().writeLine("Uang kamu       : M" + to_string(debtor->getCurrency()));
    GameManager::getInstance().writeLine("Total kewajiban : M" + to_string(amount));

    int maxLiq = debtor->getMaxLiquidatableValue(&board);
    int totalAsetUang = debtor->getCurrency() + maxLiq;

    if (totalAsetUang < amount) {
        GameManager::getInstance().writeLine("");
        GameManager::getInstance().writeLine("Estimasi dana maksimum dari likuidasi:");
        GameManager::getInstance().writeLine("  Jual semua properti + bangunan -> M" + to_string(maxLiq));
        GameManager::getInstance().writeLine("Total aset + uang tunai          : M" + to_string(totalAsetUang));
        GameManager::getInstance().writeLine("Tidak cukup untuk menutup kewajiban M" + to_string(amount) + ".");
        GameManager::getInstance().writeLine("");

        GameManager::getInstance().writeLine(debtor->getUsername() + " dinyatakan BANGKRUT!");
        assetAcquisition(debtor, creditor);
        logger.log(debtor->getUsername(), StateLog::BANKRUPT, "Pemain dinyatakan bangkrut dan keluar dari permainan");
        return;
    }

    GameManager::getInstance().writeLine("Kekurangan      : M" + to_string(amount - debtor->getCurrency()));
    GameManager::getInstance().writeLine("");
    GameManager::getInstance().writeLine("Estimasi dana maksimum dari likuidasi:");

    int totalPotensiDisplay = 0;
    vector<Property*> debtorProps;
    for (Tile* t : board.getTiles()) {
        Property* p = dynamic_cast<Property*>(t);
        if (p != nullptr && p->getOwner() == debtor) {
            debtorProps.push_back(p);
            
            if (p->getPropertyStatus() == OWNED) {
                int propSellVal = p->getLandCost();
                Street* street = dynamic_cast<Street*>(p);
                if (street) propSellVal += (street->getBuildingValue() / 2);
                
                GameManager::getInstance().writeLine("  Jual " + p->getName() + " (" + p->getCode() + ")   [" + p->getColor() + "]   -> M" + to_string(propSellVal));
                totalPotensiDisplay += propSellVal;
            } else if (p->getPropertyStatus() == MORTGAGED) {
                GameManager::getInstance().writeLine("  Gadai " + p->getName() + "  (" + p->getCode() + ")  [" + p->getColor() + "]    -> M" + to_string(p->getMortgageValue()));
            }
        }
    }
    
    GameManager::getInstance().writeLine("  Total potensi        -> M" + to_string(totalPotensiDisplay));
    GameManager::getInstance().writeLine("");

    GameManager::getInstance().writeLine("Dana likuidasi dapat menutup kewajiban.");
    GameManager::getInstance().writeLine("Kamu wajib melikuidasi aset untuk membayar.");


    string dest = creditor ? creditor->getUsername() : "Bank";
    GameManager::getInstance().writeLine("");
    GameManager::getInstance().writeLine("Kewajiban M" + to_string(amount) + " terpenuhi. Membayar ke " + dest + "...");
    
    int prevMoney = debtor->getCurrency();
    *debtor -= amount;
    
    GameManager::getInstance().writeLine("Uang kamu : M" + to_string(prevMoney) + " -> M" + to_string(debtor->getCurrency()));
    
    if (creditor) {
        int prevCreditorMoney = creditor->getCurrency();
        *creditor += amount;
        GameManager::getInstance().writeLine("Uang " + creditor->getUsername() + ": M" + to_string(prevCreditorMoney) + " -> M" + to_string(creditor->getCurrency()));
    }
}

void GameManager::assetAcquisition(Player* debtor, Player* creditor) {
    if (debtor == nullptr) return;

    vector<Property*> debtorProps;
    for (Tile* t : board.getTiles()) {
        Property* p = dynamic_cast<Property*>(t);
        if (p != nullptr && p->getOwner() == debtor) {
            debtorProps.push_back(p);
        }
    }

    string kreditorName = creditor ? creditor->getUsername() : "Bank";
    GameManager::getInstance().writeLine("Kreditor: " + kreditorName);
    GameManager::getInstance().writeLine("");

    if (creditor != nullptr) {
        GameManager::getInstance().writeLine("Pengalihan aset ke " + creditor->getUsername() + ":");
        GameManager::getInstance().writeLine("  - Uang tunai sisa  : M" + to_string(debtor->getCurrency()));
        
        if (debtor->getCurrency() > 0) {
            *creditor += debtor->getCurrency();
            debtor->setCurrency(0);
        }

        for (Property* p : debtorProps) {
            string propStatus = "  - " + p->getName() + " (" + p->getCode() + ")    [" + p->getColor() + "]  ";
            if (p->getPropertyStatus() == OWNED) {
                propStatus += "OWNED";
                Street* street = dynamic_cast<Street*>(p);
                if (street && street->getCurrentLevel() > 0) {
                    propStatus += " (" + to_string(street->getCurrentLevel()) + " bangunan)";
                }
            } else if (p->getPropertyStatus() == MORTGAGED) {
                propStatus += "MORTGAGED [M]";
            }
            GameManager::getInstance().writeLine(propStatus);
            
            p->setOwner(creditor);
        }
        GameManager::getInstance().writeLine("");
        GameManager::getInstance().writeLine(creditor->getUsername() + " menerima semua aset " + debtor->getUsername() + ".");

    } else {
        GameManager::getInstance().writeLine("Uang sisa M" + to_string(debtor->getCurrency()) + " diserahkan ke Bank.");
        debtor->setCurrency(0);

        GameManager::getInstance().writeLine("Seluruh properti dikembalikan ke status BANK.");
        GameManager::getInstance().writeLine("Bangunan dihancurkan — stok dikembalikan ke Bank.");

        if (!debtorProps.empty()) {
            GameManager::getInstance().writeLine("");
            GameManager::getInstance().writeLine("Properti akan dilelang satu per satu:");
            for (Property* p : debtorProps) {
                GameManager::getInstance().writeLine("  -> Lelang: " + p->getName() + " (" + p->getCode() + ") ...");
                
                Street* street = dynamic_cast<Street*>(p);
                if (street != nullptr) {
                    street->setCurrentLevel(0); 
                }

                p->setOwner(nullptr);
                p->setPropertyStatus(BANK);
                
                // Eksekusi fungsi lelang
                auction(p); 
            }
        }
    }

    debtor->setCurrentStatus(BANKRUPT);
    
    int activePlayerCount = 0;
    for (Player* p : players) {
        if (p != nullptr && p->getStatus() != BANKRUPT) activePlayerCount++;
    }

    GameManager::getInstance().writeLine("");
    GameManager::getInstance().writeLine(debtor->getUsername() + " telah keluar dari permainan.");
    if (activePlayerCount <= 1) {
        GameManager::getInstance().writeLine("Permainan berakhir karena hanya tersisa 1 pemain!");
    } else {
        GameManager::getInstance().writeLine("Permainan berlanjut dengan " + to_string(activePlayerCount) + " pemain tersisa.");
    }
}