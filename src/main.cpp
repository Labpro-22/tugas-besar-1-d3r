#include "../include/core/DataManager.hpp"
#include "../include/core/GameManager.hpp"

#include <exception>
#include <iostream>

static void runCli(GameManager& gm) {
    gm.getBoard().printBoard();
    std::cout << "[INFO] Running CLI." << std::endl;
}

static void runGui(GameManager& gm) {
    (void)gm;
    std::cout << "[WARN] GUI route requested, but this build is CLI-only." << std::endl;
}

int main() {
    try {
        const bool useGui = false;

        GameManager& gm = GameManager::getInstance();

        std::string defaultConfigPath = "config/";
        DataManager dm(
            defaultConfigPath + "misc.txt",
            defaultConfigPath + "property.txt",
            defaultConfigPath + "tax.txt",
            defaultConfigPath + "utility.txt",
            defaultConfigPath + "railroad.txt",
            defaultConfigPath + "special.txt",
            defaultConfigPath + "aksi.txt");

        std::cout << "[INFO] Loading game data..." << std::endl;
        dm.load();
        std::cout << "[SUCCESS] Game data loaded successfully!" << std::endl;

        if (useGui) {
            runGui(gm);
        } else {
            runCli(gm);
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "[ERROR] Unknown exception occurred!" << std::endl;
        return 1;
    }
}
