#include "../../include/core/CommandHandler.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/DataManager.hpp"
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
            try {
                if (value < minValue || value > maxValue) {
                    throw InvalidChoiceException(value, minValue, maxValue);
                }
                return value;
            } catch (const InvalidChoiceException& e) {
                GameManager::getInstance().writeLine(
                    "Masukkan angka antara " + std::to_string(e.getMinChoice()) + " dan " + std::to_string(e.getMaxChoice()) + "."
                );
                continue;
            }
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

    auto formatMoney = [](int amount) {
        std::string digits = std::to_string(amount);
        for (int i = static_cast<int>(digits.length()) - 3; i > 0; i -= 3) {
            digits.insert(static_cast<size_t>(i), ".");
        }
        return std::string("M") + digits;
    };

    auto formatUpperLabel = [](std::string raw) {
        std::replace(raw.begin(), raw.end(), '_', ' ');
        return raw;
    };

    auto formatPropertyName = [](std::string raw) {
        const bool shortUpperAcronym =
            raw.find('_') == std::string::npos &&
            raw.length() <= 4 &&
            std::all_of(raw.begin(), raw.end(), [](unsigned char c) {
                return !std::isalpha(c) || std::isupper(c);
            });

        if (shortUpperAcronym) {
            return raw;
        }

        std::replace(raw.begin(), raw.end(), '_', ' ');
        bool newWord = true;
        for (char& c : raw) {
            unsigned char uc = static_cast<unsigned char>(c);
            if (c == ' ') {
                newWord = true;
                continue;
            }

            c = static_cast<char>(newWord ? std::toupper(uc) : std::tolower(uc));
            newWord = false;
        }

        return raw;
    };

    auto getPropertyGroupLabel = [&formatUpperLabel](Property* property) {
        if (dynamic_cast<Railroad*>(property) != nullptr) {
            return std::string("STASIUN");
        }
        if (dynamic_cast<Utility*>(property) != nullptr) {
            return std::string("UTILITAS");
        }
        return formatUpperLabel(property->getColor());
    };

    auto getPropertyDisplayName = [&formatPropertyName](Property* property) {
        return formatPropertyName(property->getName()) + " (" + property->getCode() + ")";
    };

    if (command == "CETAK_PAPAN") {
        game.getBoard().printBoard();
    } else if (command == "CETAK_PROPERTI") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer == nullptr) {
            game.writeLine("Game belum diinisialisasi. Current player belum ada.");
            return true;
        }
        try {
            currentPlayer->printProperties();
        } catch (const NoPropertyException&) {
            game.writeLine("Kamu belum memiliki properti apapun.");
        }
    } else if (command == "GADAI") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer == nullptr) {
            game.writeLine("Game belum diinisialisasi. Current player belum ada.");
            return true;
        }

        std::vector<Property*> candidates;
        for (Tile* tile : game.getBoard().getTiles()) {
            Property* property = dynamic_cast<Property*>(tile);
            if (property != nullptr &&
                property->getOwner() == currentPlayer &&
                property->getPropertyStatus() == OWNED) {
                candidates.push_back(property);
            }
        }

        try {
            if (candidates.empty()) {
                throw NoPropertyToMortgageException();
            }
        } catch (const NoPropertyToMortgageException&) {
            game.writeLine("Tidak ada properti yang dapat digadaikan saat ini.");
            return true;
        }

        game.writeLine("=== Properti yang Dapat Digadaikan ===");
        for (size_t i = 0; i < candidates.size(); ++i) {
            std::ostringstream row;
            row << std::to_string(i + 1) << ". "
                << std::left << std::setw(27) << getPropertyDisplayName(candidates[i])
                << std::left << std::setw(12) << ("[" + getPropertyGroupLabel(candidates[i]) + "]")
                << " Nilai Gadai: " << formatMoney(candidates[i]->getMortgageValue());
            game.writeLine(row.str());
        }
        game.writeLine("");

        const int pick = askInt("Pilih nomor properti (0 untuk batal): ", 0, static_cast<int>(candidates.size()));
        if (pick == 0) {
            return true;
        }

        Property* selected = candidates[static_cast<size_t>(pick - 1)];

        try {
            currentPlayer->mortgageProperty(selected, &game.getBoard());
            Logger::getInstance().log(
                currentPlayer->getUsername(),
                StateLog::PAY_MORTGAGE,
                "Menggadaikan " + selected->getName() + " (" + selected->getCode() +
                ") senilai " + formatMoney(selected->getMortgageValue())
            );

            game.writeLine(formatPropertyName(selected->getName()) + " berhasil digadaikan.");
            game.writeLine("Kamu menerima " + formatMoney(selected->getMortgageValue()) + " dari Bank.");
            game.writeLine("Uang kamu sekarang: " + formatMoney(currentPlayer->getCurrency()));
            game.writeLine("Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.");
        } catch (const NoPropertyToMortgageException&) {
            game.writeLine("Tidak ada properti yang dapat digadaikan saat ini.");
        } catch (const FailedMortgageException& e) {
            game.writeLine(formatPropertyName(selected->getName()) + " tidak dapat digadaikan!");
            game.writeLine("Masih terdapat bangunan di color group [" + formatUpperLabel(e.getColorGroup()) + "].");
            game.writeLine("Bangunan harus dijual terlebih dahulu.");
            game.writeLine("");

            std::vector<Street*> built;
            for (Tile* tile : game.getBoard().getColorGroup(e.getColorGroup())) {
                Street* street = dynamic_cast<Street*>(tile);
                if (street != nullptr &&
                    street->getOwner() == currentPlayer &&
                    street->getCurrentLevel() > 0) {
                    built.push_back(street);
                }
            }

            if (built.empty()) {
                return true;
            }

            game.writeLine("Daftar bangunan di color group [" + formatUpperLabel(e.getColorGroup()) + "]:");
            for (size_t i = 0; i < built.size(); ++i) {
                std::string buildingState = built[i]->getCurrentLevel() == 5
                    ? "Hotel"
                    : std::to_string(built[i]->getCurrentLevel()) + " rumah";

                std::ostringstream row;
                row << std::to_string(i + 1) << ". "
                    << std::left << std::setw(30) << getPropertyDisplayName(built[i])
                    << " - " << std::left << std::setw(8) << buildingState
                    << " -> Nilai jual bangunan: " << formatMoney(built[i]->getBuildingValue() / 2);
                game.writeLine(row.str());
            }
            game.writeLine("");

            const std::string confirmSell = askChoice(
                "Jual semua bangunan color group [" + formatUpperLabel(e.getColorGroup()) + "]? (y/n): ",
                {"y", "n"}
            );
            if (confirmSell == "n") {
                return true;
            }

            for (Street* street : built) {
                const int gain = street->getBuildingValue() / 2;
                street->setCurrentLevel(0);
                *currentPlayer += gain;
                game.writeLine(
                    "Bangunan " + formatPropertyName(street->getName()) +
                    " terjual. Kamu menerima " + formatMoney(gain) + "."
                );
            }

            game.writeLine("Uang kamu sekarang: " + formatMoney(currentPlayer->getCurrency()));
            game.writeLine("");

            const std::string confirmMortgage = askChoice(
                "Lanjut menggadaikan " + formatPropertyName(selected->getName()) + "? (y/n): ",
                {"y", "n"}
            );
            if (confirmMortgage == "n") {
                return true;
            }

            try {
                currentPlayer->mortgageProperty(selected, &game.getBoard());
                Logger::getInstance().log(
                    currentPlayer->getUsername(),
                    StateLog::PAY_MORTGAGE,
                    "Menggadaikan " + selected->getName() + " (" + selected->getCode() +
                    ") senilai " + formatMoney(selected->getMortgageValue())
                );

                game.writeLine(formatPropertyName(selected->getName()) + " berhasil digadaikan.");
                game.writeLine("Kamu menerima " + formatMoney(selected->getMortgageValue()) + " dari Bank.");
                game.writeLine("Uang kamu sekarang: " + formatMoney(currentPlayer->getCurrency()));
                game.writeLine("Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.");
            } catch (const NoPropertyToMortgageException&) {
                game.writeLine("Tidak ada properti yang dapat digadaikan saat ini.");
            } catch (const FailedMortgageException&) {
                game.writeLine(formatPropertyName(selected->getName()) + " tidak dapat digadaikan!");
            }
        }
    } else if (command == "TEBUS") {
        Player* currentPlayer = game.getCurrentTurnPlayer();
        if (currentPlayer == nullptr) {
            game.writeLine("Game belum diinisialisasi. Current player belum ada.");
            return true;
        }

        std::vector<Property*> mortgaged;
        for (Tile* tile : game.getBoard().getTiles()) {
            Property* property = dynamic_cast<Property*>(tile);
            if (property != nullptr &&
                property->getOwner() == currentPlayer &&
                property->getPropertyStatus() == MORTGAGED) {
                mortgaged.push_back(property);
            }
        }

        try {
            if (mortgaged.empty()) {
                throw NoMortgageException();
            }
        } catch (const NoMortgageException&) {
            game.writeLine("Tidak ada properti yang sedang digadaikan.");
            return true;
        }

        game.writeLine("=== Properti yang Sedang Digadaikan ===");
        for (size_t i = 0; i < mortgaged.size(); ++i) {
            std::ostringstream row;
            row << std::to_string(i + 1) << ". "
                << std::left << std::setw(27) << getPropertyDisplayName(mortgaged[i])
                << std::left << std::setw(12) << ("[" + getPropertyGroupLabel(mortgaged[i]) + "]")
                << " [M]  Harga Tebus: " << formatMoney(mortgaged[i]->getLandCost());
            game.writeLine(row.str());
        }
        game.writeLine("");

        game.writeLine("Uang kamu saat ini: " + formatMoney(currentPlayer->getCurrency()));
        const int pick = askInt("Pilih nomor properti (0 untuk batal): ", 0, static_cast<int>(mortgaged.size()));
        if (pick == 0) {
            return true;
        }

        Property* selected = mortgaged[static_cast<size_t>(pick - 1)];

        try {
            currentPlayer->buyBackMortgaged(selected);
            Logger::getInstance().log(
                currentPlayer->getUsername(),
                StateLog::PAY_MORTGAGE,
                "Menebus " + selected->getName() + " (" + selected->getCode() +
                ") seharga " + formatMoney(selected->getLandCost())
            );

            game.writeLine(formatPropertyName(selected->getName()) + " berhasil ditebus!");
            game.writeLine("Kamu membayar " + formatMoney(selected->getLandCost()) + " ke Bank.");
            game.writeLine("Uang kamu sekarang: " + formatMoney(currentPlayer->getCurrency()));
        } catch (const NoMortgageException&) {
            game.writeLine("Tidak ada properti yang sedang digadaikan.");
        } catch (const NotEnoughMoneyException&) {
            game.writeLine("Uang kamu tidak cukup untuk menebus " + formatPropertyName(selected->getName()) + ".");
            game.writeLine(
                "Harga tebus: " + formatMoney(selected->getLandCost()) +
                " | Uang kamu: " + formatMoney(currentPlayer->getCurrency())
            );
        }
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

        try {
            if (!currentPlayer->getCanUseCard()) {
                throw AbilityUsedException();
            }
        } catch (const AbilityUsedException&) {
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
        game.writeLine("Commands: CETAK_PAPAN,\nCETAK_PROPERTI,\nGADAI,\nTEBUS,\nBANGUN,\nLEMPAR_DADU,\nATUR_DADU X Y,\nSTATUS,\nCETAK_KARTU,\nGUNAKAN_KEMAMPUAN,\nSIMPAN,\nMUAT,\nKELUAR");
    }
    // SAVE/LOAD
    else if (command == "SIMPAN") {
        string first;
        if (iss >> first) {
            DataManager dm;
            bool isOverride = false;
            while(true) {
                try {
                    game.writeLine("Menyimpan permainan...");
                    dm.save(first, isOverride);
                    game.writeLine("Permainan berhasil disimpan ke: " + first);
                } catch (SaveProhibitedException& e) {
                    game.writeLine("Command SIMPAN tidak bisa dilakukan jika telah melakukan suatu aksi.");
                } catch (FileExistsException& e) {
                    game.writeLine("File \"" + e.getFileName() + "\" sudah ada.");
                    string confirmation = askInput("Timpa file lama? (y/n): ");
                    if(confirmation == "y" || confirmation == "Y") {
                        isOverride = true;
                        continue;
                    }
                    game.writeLine("SIMPAN dibatalkan.");
                } catch (SaveFailedException& e) {
                    game.writeLine("Gagal menyimpan file! Pastikan direktori dapat ditulis.");
                }
                break;
            }
        }

        else {
            game.writeLine("Format: SIMPAN <nama_file>.txt");
        }
    } else if (command == "MUAT") {
        // not implemented yet
    }
    
    else if (command == "KELUAR") {
        return false;
    }
    else if (command == "CETAK_AKTA") {
        const std::string code = askInput("Masukkan kode petak: ");
        game.getBoard().cetakAkta(code);
    
    } else if (!command.empty()) {
        game.writeLine("Command tidak dikenali. Ketik BANTUAN untuk melihat command yang dikenali.");
    } 
    

    return true;
}
