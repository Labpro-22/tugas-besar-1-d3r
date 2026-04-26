#include "../../include/core/GameManager.hpp"

#include <algorithm>
#include <iostream>
#include <string>

#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"
#include "../../include/gui/UIOverlay/UIComponent.hpp"

using namespace std;

GameManager::GameManager() 
    : turn(0), maxTurn(0), activePlayerCount(0), playerCount(0), initialCurrency(0),
      board(45), currentTurnPlayer(nullptr), useGuiStream(false), tilePopup(nullptr) {
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
    if (useGuiStream && inputCallback) {
        return inputCallback(prompt);
    }

    if (!prompt.empty()) {
        write(prompt);
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
        writeLine("[WARN] Game state is not valid yet.");
        return;
    }

    writeLine("[INFO] Game ready.");
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

    try {
        dice.setValues(dice1, dice2);
    } catch (const InvalidDiceException&) {
        writeLine("Nilai dadu harus 1 sampai 6.");
        return;
    }

    int total = dice.getTotal();
    Tile* destination = board.goToTile(*currentTurnPlayer->getCurrentTile(), total);

    writeLine("Hasil: " + std::to_string(dice.getFirst()) + " + " + std::to_string(dice.getSecond()) + " = " + std::to_string(total));
    if (destination == nullptr) {
        writeLine("Tujuan tidak valid.");
        return;
    }

    currentTurnPlayer->moveTo(destination, true, FORWARD);
    TilePopup popUpcaller;
    destination->callPopUp(popUpcaller);
    // writeLine("Mendarat di: " + destination->getName() + " (" + destination->getCode() + ")");
    
    if (!dice.isDouble()) {
        nextTurn();
    } else {
        writeLine("Double. Pemain mendapat giliran tambahan.");
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
        board.advanceFestivalEffects(currentTurnPlayer);
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

void GameManager::pay(Player *debtor, int amount, Player* creditor) {
    if (debtor == nullptr || amount <= 0) {
        return;
    }

    try{
        if (debtor->getCurrency() < amount) {
            throw NotEnoughMoneyException("", amount, debtor->getCurrency());
        }

        *debtor -= amount;
        if (creditor){
            *creditor += amount;
        }
    }
    catch(const NotEnoughMoneyException& e) {
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
    if (debtor == nullptr || amount <= 0) {
        return;
    }

    GameManager::getInstance().writeLine("");
    string target = creditor ? (" kepada " + creditor->getUsername()) : "";
    string reason = creditor ? "sewa M" : "tagihan M";
    if (!creditor && debtor->getCurrentTile() != nullptr) {
        reason = debtor->getCurrentTile()->getName() + " M";
    }
    
    GameManager::getInstance().writeLine("Kamu tidak dapat membayar " + reason + to_string(amount) + target + "!");
    GameManager::getInstance().writeLine("");

    GameManager::getInstance().writeLine("Uang kamu       : M" + to_string(debtor->getCurrency()));
    GameManager::getInstance().writeLine("Total kewajiban : M" + to_string(amount));
    if (debtor->getCurrency() < amount) {
        GameManager::getInstance().writeLine("Kekurangan      : M" + to_string(amount - debtor->getCurrency()));
    }

    int totalAsetUang = debtor->getMaxLiquidatableValue(&board);
    int totalPotensiLikuidasi = totalAsetUang - debtor->getCurrency();
    if (totalPotensiLikuidasi < 0) {
        totalPotensiLikuidasi = 0;
    }

    if (totalAsetUang < amount) {
        GameManager::getInstance().writeLine("");
        GameManager::getInstance().writeLine("Estimasi dana maksimum dari likuidasi:");
        GameManager::getInstance().writeLine("  Total potensi likuidasi       -> M" + to_string(totalPotensiLikuidasi));
        GameManager::getInstance().writeLine("Total aset + uang tunai          : M" + to_string(totalAsetUang));
        GameManager::getInstance().writeLine("Tidak cukup untuk menutup kewajiban M" + to_string(amount) + ".");
        GameManager::getInstance().writeLine("");

        GameManager::getInstance().writeLine(debtor->getUsername() + " dinyatakan BANGKRUT!");
        assetAcquisition(debtor, creditor);
        return;
    }

    GameManager::getInstance().writeLine("");
    GameManager::getInstance().writeLine("Estimasi dana maksimum dari likuidasi:");

    int totalPotensiDisplay = 0;
    vector<Property*> debtorProps = debtor->getOwnedProperties();
    for (Property* p : debtorProps) {
        if (p == nullptr || p->getPropertyStatus() != OWNED) {
            continue;
        }

        int propSellVal = p->getLandCost();
        Street* street = dynamic_cast<Street*>(p);
        if (street != nullptr) {
            propSellVal += (street->getBuildingValue() / 2);
        }

        bool canMortgage = true;
        vector<Tile*> colorGroupProperties = board.getColorGroup(p->getColor());
        for (Tile* colorTile : colorGroupProperties) {
            Property* owned = dynamic_cast<Property*>(colorTile);
            if (owned == nullptr || owned->getOwner() != debtor) {
                continue;
            }

            Street* streetOwned = dynamic_cast<Street*>(owned);
            if (streetOwned != nullptr && streetOwned->getCurrentLevel() > 0) {
                canMortgage = false;
                break;
            }
        }

        string group = p->getColor();
        if (dynamic_cast<Railroad*>(p) != nullptr) {
            group = "STASIUN";
        } else if (dynamic_cast<Utility*>(p) != nullptr) {
            group = "UTILITAS";
        }

        if (canMortgage && p->getMortgageValue() > propSellVal) {
            GameManager::getInstance().writeLine("  Gadai " + p->getName() + " (" + p->getCode() + ") [" + group + "] -> M" + to_string(p->getMortgageValue()));
            totalPotensiDisplay += p->getMortgageValue();
        } else {
            GameManager::getInstance().writeLine("  Jual " + p->getName() + " (" + p->getCode() + ") [" + group + "] -> M" + to_string(propSellVal));
            totalPotensiDisplay += propSellVal;
        }
    }
    
    GameManager::getInstance().writeLine("  Total potensi        -> M" + to_string(totalPotensiDisplay));
    GameManager::getInstance().writeLine("");

    GameManager::getInstance().writeLine("Dana likuidasi dapat menutup kewajiban.");
    GameManager::getInstance().writeLine("Kamu wajib melikuidasi aset untuk membayar.");

    CommandHandler& handler = GameManager::getInstance().getCommandHandler();
    while (debtor->getCurrency() < amount) {
        vector<Property*> sellOptions;
        vector<Property*> mortgageOptions;
        debtorProps = debtor->getOwnedProperties();

        for (Property* p : debtorProps) {
            if (p == nullptr || p->getPropertyStatus() != OWNED) {
                continue;
            }

            int propSellVal = p->getLandCost();
            Street* street = dynamic_cast<Street*>(p);
            if (street != nullptr) {
                propSellVal += (street->getBuildingValue() / 2);
            }

            bool canMortgage = true;
            vector<Tile*> colorGroupProperties = board.getColorGroup(p->getColor());
            for (Tile* colorTile : colorGroupProperties) {
                Property* owned = dynamic_cast<Property*>(colorTile);
                if (owned == nullptr || owned->getOwner() != debtor) {
                    continue;
                }

                Street* streetOwned = dynamic_cast<Street*>(owned);
                if (streetOwned != nullptr && streetOwned->getCurrentLevel() > 0) {
                    canMortgage = false;
                    break;
                }
            }

            if (canMortgage && p->getMortgageValue() > propSellVal) {
                mortgageOptions.push_back(p);
            } else {
                sellOptions.push_back(p);
            }
        }

        if (sellOptions.empty() && mortgageOptions.empty()) {
            break;
        }

        GameManager::getInstance().writeLine("");
        GameManager::getInstance().writeLine("=== Panel Likuidasi ===");
        GameManager::getInstance().writeLine("Uang kamu saat ini: M" + to_string(debtor->getCurrency()) + "  |  Kewajiban: M" + to_string(amount));
        GameManager::getInstance().writeLine("");

        int optionCount = 0;
        if (!sellOptions.empty()) {
            GameManager::getInstance().writeLine("[Jual ke Bank]");
            for (size_t i = 0; i < sellOptions.size(); ++i) {
                Property* p = sellOptions[i];
                string group = p->getColor();
                if (dynamic_cast<Railroad*>(p) != nullptr) {
                    group = "STASIUN";
                } else if (dynamic_cast<Utility*>(p) != nullptr) {
                    group = "UTILITAS";
                }

                int propSellVal = p->getLandCost();
                Street* street = dynamic_cast<Street*>(p);
                string extraInfo = "";
                if (street != nullptr) {
                    int buildingValue = street->getBuildingValue() / 2;
                    propSellVal += buildingValue;
                    if (street->getCurrentLevel() > 0) {
                        extraInfo = " (termasuk " + to_string(street->getCurrentLevel());
                        extraInfo += street->getCurrentLevel() == 5 ? " hotel: M" : " rumah: M";
                        extraInfo += to_string(buildingValue) + ")";
                    }
                }

                GameManager::getInstance().writeLine(
                    to_string(optionCount + 1) + ". " + p->getName() + " (" + p->getCode() + ") [" + group + "] Harga Jual: M" +
                    to_string(propSellVal) + extraInfo
                );
                optionCount++;
            }
            GameManager::getInstance().writeLine("");
        }

        if (!mortgageOptions.empty()) {
            GameManager::getInstance().writeLine("[Gadaikan]");
            for (size_t i = 0; i < mortgageOptions.size(); ++i) {
                Property* p = mortgageOptions[i];
                string group = p->getColor();
                if (dynamic_cast<Railroad*>(p) != nullptr) {
                    group = "STASIUN";
                } else if (dynamic_cast<Utility*>(p) != nullptr) {
                    group = "UTILITAS";
                }

                GameManager::getInstance().writeLine(
                    to_string(optionCount + 1) + ". " + p->getName() + " (" + p->getCode() + ") [" + group + "] Nilai Gadai: M" +
                    to_string(p->getMortgageValue())
                );
                optionCount++;
            }
            GameManager::getInstance().writeLine("");
        }

        int pick = handler.askInt("Pilih aksi (0 jika sudah cukup): ", 0, optionCount);
        if (pick == 0) {
            if (debtor->getCurrency() >= amount) {
                break;
            }

            GameManager::getInstance().writeLine("Dana kamu masih belum cukup. Likuidasi harus dilanjutkan.");
            continue;
        }

        if (pick <= static_cast<int>(sellOptions.size())) {
            Property* selected = sellOptions[static_cast<size_t>(pick - 1)];
            int received = selected->getLandCost();
            Street* street = dynamic_cast<Street*>(selected);
            if (street != nullptr) {
                received += street->getBuildingValue() / 2;
            }

            sellPropertyToBank(debtor, selected);
            GameManager::getInstance().writeLine("");
            GameManager::getInstance().writeLine(selected->getName() + " terjual ke Bank. Kamu menerima M" + to_string(received) + ".");
            GameManager::getInstance().writeLine("Uang kamu sekarang: M" + to_string(debtor->getCurrency()));
        } else {
            Property* selected = mortgageOptions[static_cast<size_t>(pick - static_cast<int>(sellOptions.size()) - 1)];
            try {
                debtor->mortgageProperty(selected, &board);
                GameManager::getInstance().writeLine("");
                GameManager::getInstance().writeLine(selected->getName() + " berhasil digadaikan.");
                GameManager::getInstance().writeLine("Kamu menerima M" + to_string(selected->getMortgageValue()) + " dari Bank.");
                GameManager::getInstance().writeLine("Uang kamu sekarang: M" + to_string(debtor->getCurrency()));
                GameManager::getInstance().writeLine("Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.");
            } catch (const NimonspoliException&) {
                GameManager::getInstance().writeLine("Properti tidak dapat digadaikan saat ini.");
            }
        }
    }

    if (debtor->getCurrency() < amount) {
        GameManager::getInstance().writeLine("");
        GameManager::getInstance().writeLine("Seluruh opsi likuidasi telah habis, tetapi dana masih belum cukup.");
        GameManager::getInstance().writeLine(debtor->getUsername() + " dinyatakan BANGKRUT!");
        assetAcquisition(debtor, creditor);
        return;
    }

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

    vector<Property*> debtorProps = debtor->getOwnedProperties();
    debtor->setCurrentStatus(BANKRUPT);

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
                
                auction(p); 
            }
        }
    }
    
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
