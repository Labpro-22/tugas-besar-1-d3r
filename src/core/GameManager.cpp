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
    cout << "\nKamu tidak dapat membayar " << (creditor ? "sewa " : "tagihan ") 
              << "M" << amount << " kepada " << (creditor ? creditor->getUsername() : "Bank") << "!" << endl << endl;

    cout << "Uang kamu       : M" << debtor->getCurrency() << endl;
    cout << "Total kewajiban : M" << amount << endl;
    cout << "Kekurangan      : M" << (amount - debtor->getCurrency()) << endl << endl;

    int maxLiq = debtor->getMaxLiquidatableValue(&board);
    
    cout << "Estimasi dana maksimum dari likuidasi:" << endl;
    
    int totalPotensiDisplay = 0;
    vector<Property*> debtorProps;

    for (Tile* t : board.getTiles()) {
        Property* p = dynamic_cast<Property*>(t);
        if (p != nullptr && p->getOwner() == debtor) {
            debtorProps.push_back(p);
            
            if (p->getPropertyStatus() == OWNED) {
                int propSellVal = p->getLandCost();
                string actionStr = "Jual "; 
                
                Street* street = dynamic_cast<Street*>(p);
                if (street) {
                    int bVal = street->getBuildingValue();
                    propSellVal += (bVal / 2);
                }
                
                cout << "  " << actionStr << p->getName() << " (" << p->getCode() << ")   [" << p->getColor() << "]   -> M" << propSellVal << endl;
                totalPotensiDisplay += propSellVal;
            } else if (p->getPropertyStatus() == MORTGAGED) {
                cout << "  (Tergadai) " << p->getName() << " (" << p->getCode() << ")   [" << p->getColor() << "]   -> M0\n";
            }
        }
    }
    
    cout << "  Total potensi        -> M" << totalPotensiDisplay << endl << endl;

    if (maxLiq < amount) {
        cout << "Dana likuidasi TIDAK dapat menutup kewajiban." << endl;
        cout << "Kamu dinyatakan BANGKRUT." << endl;
        
        debtor->setCurrentStatus(BANKRUPT);
        // this->handleBankruptTransfer(debtor, creditor);
        return;
    }

    cout << "Dana likuidasi dapat menutup kewajiban." << endl;
    cout << "Kamu wajib melikuidasi aset untuk membayar." << endl;

    while (debtor->getCurrency() < amount) {
        cout << "\n=== Panel Likuidasi ===" << endl;
        cout << "Uang kamu saat ini: M" << debtor->getCurrency() << "  |  Kewajiban: M" << amount << endl << endl;

        vector<Property*> listJual;
        vector<Property*> listGadai;
        for (Property* p : debtorProps) {
            if (p->getPropertyStatus() == OWNED) {
                listJual.push_back(p);
                
                listGadai.push_back(p); 
            }
        }

        int itemIndex = 1;
        
        cout << "[Jual ke Bank]" << endl;
        if (listJual.empty()) cout << "- (Tidak ada)\n";
        for (Property* p : listJual) {
            int sellPrice = p->getLandCost();
            string extraInfo = "";

            Street* street = dynamic_cast<Street*>(p);
            if (street) {
                int level = street->getCurrentLevel();
                if (level > 0) {
                    int bVal = (level >= 1 && level <= 4) ? (level * street->getHouseCost()) : 
                               ((4 * street->getHouseCost()) + street->getHotelCost());
                    int addedValue = bVal / 2;
                    sellPrice += addedValue;
                    
                    string buildStr = (level == 5) ? "1 hotel" : (to_string(level) + " rumah");
                    extraInfo = " (termasuk " + buildStr + ": M" + to_string(addedValue) + ")";
                }
            }
            cout << itemIndex++ << ". " << p->getName() << " (" << p->getCode() << ")  [" << p->getColor() << "]  Harga Jual: M" << sellPrice << extraInfo << endl;
        }

        cout << "\n[Gadaikan]" << endl;
        if (listGadai.empty()) cout << "- (Tidak ada)\n";
        for (Property* p : listGadai) {
            cout << itemIndex++ << ". " << p->getName() << " (" << p->getCode() << ")   [" << p->getColor() << "]   Nilai Gadai: M" << p->getMortgageValue() << endl;
        }

        int maxPilihan = itemIndex - 1;
        int pilihan = getCommandHandler().askInt("\nPilih aksi (0 jika sudah cukup): ", 0, maxPilihan);

        if (pilihan == 0) {
            if (debtor->getCurrency() < amount) {
                cout << "Uang kamu masih belum cukup untuk membayar tagihan. Kamu harus menjual/menggadai properti!" << endl;
                continue;
            } else {
                break; 
            }
        }

        if (pilihan <= listJual.size()) {
            Property* pTarget = listJual[pilihan - 1];
            
            int prevMoney = debtor->getCurrency();
            sellPropertyToBank(debtor, pTarget);
            int earned = debtor->getCurrency() - prevMoney;
            
            cout << "\n" << pTarget->getName() << " terjual ke Bank. Kamu menerima M" << earned << "." << endl;
            cout << "Uang kamu sekarang: M" << debtor->getCurrency() << endl;
            
        } else {
            Property* pTarget = listGadai[pilihan - listJual.size() - 1];
            
            try {
                int prevMoney = debtor->getCurrency();
                
                debtor->mortgageProperty(pTarget, &board); 
                
                int earned = debtor->getCurrency() - prevMoney;
                cout << "\n" << pTarget->getName() << " berhasil digadaikan. Kamu menerima M" << earned << "." << endl;
                cout << "Uang kamu sekarang: M" << debtor->getCurrency() << endl;

            } catch (const exception& e) {
                cout << "\nGagal menggadai: " << e.what() << endl;
            }
        }
    }  
}