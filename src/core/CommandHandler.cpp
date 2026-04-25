#include "../../include/core/CommandHandler.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"

static std::string normalizeToken(std::string text, bool caseInsensitive)
{
    if (!caseInsensitive) {
        return text;
    }

    std::transform(text.begin(), text.end(), text.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    return text;
}

static std::string joinOptions(const std::vector<std::string>& options)
{
    std::string result;
    for (size_t i = 0; i < options.size(); ++i) {
        if (i > 0) {
            result += "/";
        }
        result += options[i];
    }
    return result;
}

std::string CommandHandler::askInput(const std::string& prompt, bool allowEmpty) const {
    while (true) {
        const std::string line = GameManager::getInstance().readLine(prompt);
        if (allowEmpty || !line.empty()) {
            return line;
        }

        GameManager::getInstance().writeLine("Input tidak boleh kosong.");
    }
}

std::string CommandHandler::askChoice(const std::string& prompt,const std::vector<std::string>& validOptions,bool caseInsensitive) const {
    while (true) {
        const std::string line = askInput(prompt);
        const std::string normalizedLine = normalizeToken(line, caseInsensitive);

        for (const std::string& option : validOptions) {
            if (normalizedLine == normalizeToken(option, caseInsensitive)) {
                return option;
            }
        }

        GameManager::getInstance().writeLine(
            "Pilihan tidak valid. Opsi yang tersedia: " + joinOptions(validOptions)
        );
    }
}

int CommandHandler::askInt(const std::string& prompt, int minValue, int maxValue) const {
    while (true) {
        const std::string line = askInput(prompt);

        std::istringstream iss(line);
        int value = 0;
        char tail = '\0';

        if ((iss >> value) && !(iss >> tail)) {
            if (value < minValue || value > maxValue) {
                GameManager::getInstance().writeLine(
                    "Masukkan angka antara " + std::to_string(minValue) + " dan " + std::to_string(maxValue) + "."
                );
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
    } else if (command == "CETAK_PROPERTI") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer == nullptr) {
            game.writeLine("Game belum diinisialisasi. Current player belum ada.");
            return true;
        }
        currentPlayer->printProperties();
    } else if (command == "BANGUN") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer == nullptr) {
            game.writeLine("Game belum diinisialisasi. Current player belum ada.");
            return true;
        }

        std::vector<Street*> buildable;
        for (Tile* tile : game.getBoard().getTiles()) {
            Street* street = dynamic_cast<Street*>(tile);
            if (street != nullptr && street->canBuild(*currentPlayer)) {
                buildable.push_back(street);
            }
        }

        if (buildable.empty()) {
            game.writeLine("Tidak ada properti yang memenuhi syarat untuk dibangun.");
            return true;
        }

        game.writeLine("=== Properti yang Bisa Dibangun ===");
        for (size_t i = 0; i < buildable.size(); i++) {
            Street* s = buildable[i];
            std::string state = "Level " + std::to_string(s->getCurrentLevel());
            if (s->getCurrentLevel() == 4) {
                state = "4 rumah -> Hotel";
            }
            game.writeLine(std::to_string(i + 1) + ". " + s->getName() + " (" + s->getCode() + ") [" + s->getColor() + "] | " + state + " | Biaya: M" + std::to_string(s->getBuildCost()));
        }

        int pick = 0;
        if (!(iss >> pick)) {
            pick = askInt("Pilih nomor properti (0 untuk batal): ", 0, static_cast<int>(buildable.size()));
        }

        if (pick == 0) {
            return true;
        }
        if (pick < 0 || pick > static_cast<int>(buildable.size())) {
            game.writeLine("Pilihan tidak valid.");
            return true;
        }

        Street* selected = buildable[static_cast<size_t>(pick - 1)];
        const int beforeLevel = selected->getCurrentLevel();
        const int cost = selected->getBuildCost();

        if (!selected->build(*currentPlayer)) {
            if (currentPlayer->getCurrency() < cost) {
                game.writeLine("Uang kamu tidak cukup untuk membangun.");
            } else {
                game.writeLine("Properti tidak bisa dibangun saat ini.");
            }
            return true;
        }

        if (beforeLevel == 4) {
            game.writeLine(selected->getName() + " di-upgrade ke Hotel!");
        } else {
            game.writeLine("Berhasil membangun 1 rumah di " + selected->getName() + ".");
        }
        game.writeLine("Biaya: M" + std::to_string(cost));
        game.writeLine("Uang tersisa: M" + std::to_string(currentPlayer->getCurrency()));
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
    } else if (command == "CETAK_LOG") {
        int amount;
        Logger &logger = Logger::getInstance();
        if (iss >> amount){
            logger.printLog(amount);
        } else logger.printLog();
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
        game.writeLine("Commands: CETAK_PAPAN,\nCETAK_PROPERTI,\nBANGUN,\nLEMPAR_DADU,\nATUR_DADU X Y,\nSTATUS,\nCETAK_KARTU,\nGUNAKAN_KEMAMPUAN,\nKELUAR");
    } else if (command == "KELUAR") {
        return false;
    } else if (!command.empty()) {
        game.writeLine("Command tidak dikenali. Ketik BANTUAN.");
    }

    return true;
}
