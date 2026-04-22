#include "../../include/core/CommandHandler.hpp"

#include <iostream>
#include <limits>
#include <sstream>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

std::string CommandHandler::askInput(const std::string& prompt) const {
    std::cout << prompt;

    std::string line;
    if (!std::getline(std::cin, line)) {
        return "";
    }

    return line;
}

int CommandHandler::askInt(const std::string& prompt, int minValue, int maxValue) const {
    while (true) {
        const std::string line = askInput(prompt);
        if (line.empty()) {
            return minValue;
        }

        std::istringstream iss(line);
        int value = 0;
        char tail = '\0';
        if ((iss >> value) && !(iss >> tail)) {
            if (value < minValue || value > maxValue) {
                std::cout << "Masukkan angka antara " << minValue << " dan " << maxValue << "." << std::endl;
                continue;
            }
            return value;
        }

        std::cout << "Input tidak valid. Masukkan angka." << std::endl;
    }
}

void CommandHandler::commands() {
    GameManager& game = GameManager::getInstance();

    std::string line;
    while (!game.isGameFinished()) {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer == nullptr) {
            break;
        }

        std::cout << "\n[Turn " << game.getTurn() + 1;
        if (game.getMaxTurn() > 0) {
            std::cout << "/" << game.getMaxTurn();
        }
        std::cout << "] " << currentPlayer->getUsername() << " > ";

        if (!std::getline(std::cin, line)) {
            break;
        }

        if (!execute(line)) {
            break;
        }
    }

    Player* winner = game.getWinner();
    if (winner != nullptr) {
        std::cout << "Pemenang sementara: " << winner->getUsername() << std::endl;
    }
}

bool CommandHandler::execute(const std::string& line) {
    GameManager& game = GameManager::getInstance();

    std::istringstream iss(line);
    std::string command;
    iss >> command;

    if (command == "CETAK_PAPAN") {
        game.getBoard().printBoard();
    } else if (command == "LEMPAR_DADU") {
        game.getDice().roll();
        game.rollDice(game.getDice().getFirst(), game.getDice().getSecond());
    } else if (command == "ATUR_DADU") {
        int first, second;
        if (iss >> first >> second) {
            game.rollDice(first, second);
        } else {
            std::cout << "Format: ATUR_DADU X Y" << std::endl;
        }
    } else if (command == "STATUS") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer != nullptr) {
            std::cout << currentPlayer->getUsername() << " | Uang: M" << currentPlayer->getCurrency();
            if (currentPlayer->getCurrentTile() != nullptr) {
                std::cout << " | Tile: " << currentPlayer->getCurrentTile()->getCode();
            }
            if (currentPlayer->getDiscount() > 0.0f) {
                std::cout << " | Diskon: " << currentPlayer->getDiscount() << "%";
            }
            if (currentPlayer->hasShield()) {
                std::cout << " | Shield aktif";
            }
            std::cout << std::endl;
        }
    } else if (command == "CETAK_KARTU") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer != nullptr) {
            currentPlayer->printSkillCards();
        }
    } else if (command == "GUNAKAN_KEMAMPUAN") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer == nullptr) {
            return true;
        }

        if (!currentPlayer->getCanUseCard()) {
            std::cout << "Kartu kemampuan hanya bisa digunakan 1 kali dalam 1 giliran." << std::endl;
            return true;
        }

        currentPlayer->printSkillCards();

        const int cardCount = static_cast<int>(currentPlayer->getDeck().size());
        if (cardCount <= 0) {
            return true;
        }

        int cardNumber = 0;
        if (!(iss >> cardNumber)) {
            cardNumber = askInt("Pilih kartu yang ingin digunakan (0-" + std::to_string(cardCount) + "): ", 0, cardCount);
        }

        if (cardNumber == 0) {
            return true;
        }

        SkillCard* card = currentPlayer->removeSkillCard(cardNumber - 1);
        if (card == nullptr) {
            return true;
        }

        std::cout << card->getCardName() << " used!" << std::endl;
        card->useCard(currentPlayer, game.getPlayers());
        currentPlayer->setCanUseCard(false);
        delete card;
    } 
    // else if (command == "DROP_KARTU") {
    //     // not yet
    // } 
    else if (command == "BANTUAN") {
        std::cout << "Commands: CETAK_PAPAN,\nLEMPAR_DADU,\nATUR_DADU X Y,\nSTATUS,\nCETAK_KARTU,\nGUNAKAN_KEMAMPUAN,\nKELUAR" << std::endl;
    } else if (command == "KELUAR") {
        return false;
    } else if (!command.empty()) {
        std::cout << "Command tidak dikenali. Ketik BANTUAN." << std::endl;
    }

    return true;
}
