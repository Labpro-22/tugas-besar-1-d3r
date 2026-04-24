#include "../../include/core/CommandHandler.hpp"

#include <iostream>
#include <limits>
#include <sstream>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

std::string CommandHandler::askInput(const std::string& prompt) const {
    return GameManager::getInstance().readLine(prompt);
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
                GameManager::getInstance().writeLine("Masukkan angka antara " + std::to_string(minValue) + " dan " + std::to_string(maxValue) + ".");
                continue;
            }
            return value;
        }

        GameManager::getInstance().writeLine("Input tidak valid. Masukkan angka.");
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

        std::string prompt = "\n[Turn " + std::to_string(game.getTurn() + 1);
        if (game.getMaxTurn() > 0) {
            prompt += "/" + std::to_string(game.getMaxTurn());
        }
        prompt += "] " + currentPlayer->getUsername() + " > ";

        line = game.readLine(prompt);
        if (line.empty() && (game.isGuiStreamActive() || std::cin.eof())) {
            break;
        }

        if (!execute(line)) {
            break;
        }
    }

    Player* winner = game.getWinner();
    if (winner != nullptr) {
        game.writeLine("Pemenang sementara: " + winner->getUsername());
    }
}

bool CommandHandler::execute(const std::string& line) {
    GameManager& game = GameManager::getInstance();

    // Normal command processing
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
            game.writeLine("Format: ATUR_DADU X Y");
        }
    } else if (command == "STATUS") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer != nullptr) {
            std::string status = currentPlayer->getUsername() + " | Uang: M" + std::to_string(currentPlayer->getCurrency());
            if (currentPlayer->getCurrentTile() != nullptr) {
                status += " | Tile: " + currentPlayer->getCurrentTile()->getCode();
            }
            if (currentPlayer->getDiscount() > 0.0f) {
                status += " | Diskon: " + std::to_string(currentPlayer->getDiscount()) + "%";
            }
            if (currentPlayer->hasShield()) {
                status += " | Shield aktif";
            }
            game.writeLine(status);
        } else {
            game.writeLine("Game belum diinisialisasi. Current player belum ada.");
        }
    } else if (command == "CETAK_KARTU") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer != nullptr) {
            currentPlayer->printSkillCards();
        } else {
            game.writeLine("Game belum diinisialisasi. Current player belum ada.");
        }
    } else if (command == "GUNAKAN_KEMAMPUAN") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer == nullptr) {
            game.writeLine("Game belum diinisialisasi. Current player belum ada.");
            return true;
        }

        if (!currentPlayer->getCanUseCard()) {
            game.writeLine("Kartu kemampuan hanya bisa digunakan 1 kali dalam 1 giliran.");
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

        game.writeLine(card->getCardName() + " used!");
        card->useCard(currentPlayer, game.getPlayers());
        currentPlayer->setCanUseCard(false);
        delete card;
    } 
    // else if (command == "DROP_KARTU") {
    //     // not yet
    // } 
    else if (command == "BANTUAN") {
        game.writeLine("Commands: CETAK_PAPAN,\nLEMPAR_DADU,\nATUR_DADU X Y,\nSTATUS,\nCETAK_KARTU,\nGUNAKAN_KEMAMPUAN,\nKELUAR");
    } else if (command == "KELUAR") {
        return false;
    } else if (!command.empty()) {
        game.writeLine("Command tidak dikenali. Ketik BANTUAN.");
    }

    return true;
}
