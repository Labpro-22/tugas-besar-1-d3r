#include "../../include/core/CommandHandler.hpp"

#include <iostream>
#include <sstream>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

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
        int cardNumber;
        if (iss >> cardNumber) {
            Player* currentPlayer = game.getCurrentTurnPlayer();
            if (currentPlayer == nullptr) {
                return true;
            }

            if (!currentPlayer->getCanUseCard()) {
                std::cout << "Kartu kemampuan hanya bisa digunakan 1 kali dalam 1 giliran." << std::endl;
                return true;
            }

            SkillCard* card = currentPlayer->removeSkillCard(cardNumber - 1);
            if (card == nullptr) {
                std::cout << "Nomor kartu tidak valid." << std::endl;
                return true;
            }

            card->useCard(currentPlayer, game.getPlayers());
            currentPlayer->setCanUseCard(false);
            delete card;
        } else {
            std::cout << "Format: GUNAKAN_KEMAMPUAN <nomor>" << std::endl;
        }
    } else if (command == "DROP_KARTU") {
        int cardNumber;
        if (iss >> cardNumber) {
            Player* currentPlayer = game.getCurrentTurnPlayer();
            if (currentPlayer == nullptr) {
                return true;
            }

            SkillCard* card = currentPlayer->removeSkillCard(cardNumber - 1);
            if (card == nullptr) {
                std::cout << "Nomor kartu tidak valid." << std::endl;
                return true;
            }

            delete card;
        } else {
            std::cout << "Format: DROP_KARTU <nomor>" << std::endl;
        }
    } else if (command == "BANTUAN") {
        std::cout << "Commands: CETAK_PAPAN, LEMPAR_DADU, ATUR_DADU X Y, STATUS, CETAK_KARTU, GUNAKAN_KEMAMPUAN <nomor>, DROP_KARTU <nomor>, KELUAR" << std::endl;
    } else if (command == "KELUAR") {
        return false;
    } else if (!command.empty()) {
        std::cout << "Command tidak dikenali. Ketik BANTUAN." << std::endl;
    }

    return true;
}
