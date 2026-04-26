#include "../../include/core/Board.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/Tile.hpp"
using namespace std;

Board::Board(int size): tiles(size, nullptr), tileCount(size), goIndex(0), jailIndex(-1) {
}

Tile* Board::getTile(string code) const {
    auto it = std::find_if(tiles.begin(), tiles.end(), [&code](const Tile* tile){
        return tile != nullptr && tile->getCode().compare(code) == 0;
    });

    if (it != tiles.end()) {
        return *it;
    } else {
        return nullptr;
    }
}

Tile* Board::getTile(int index) const{
    if (index < 0 || static_cast<size_t>(index) >= tiles.size()) {
        return nullptr;
    }
    return tiles[index];
}

Tile* Board::getJailTile() const {
    return getTile("PEN");
}

Tile* Board::goToTile(Tile& current, int moveAmount) const {
    auto it = std::find_if(tiles.begin(), tiles.end(), [&current](const Tile* tile){
        return tile != nullptr && tile->getCode().compare(current.getCode()) == 0;
    });
    if (it != tiles.end()) {
        int index = static_cast<int>(distance(tiles.begin(), it));
        int boardSize = static_cast<int>(tiles.size());
        int nextIndex = (index + moveAmount) % boardSize;
        if (nextIndex < 0) {
            nextIndex += boardSize;
        }
        return tiles[static_cast<size_t>(nextIndex)];
    } else {
        return nullptr;
    }
}

Tile* Board::goToTile(const std::string& code, int moveAmount) {
    Tile* currentTile = getTile(code);
    if (currentTile != nullptr) {
        return goToTile(*currentTile, moveAmount);
    }
    return nullptr;
}

std::vector<Tile*> Board::getColorGroup(const std::string& color) const {
    std::vector<Tile*> result;
    std::copy_if(tiles.begin(), tiles.end(), std::back_inserter(result), [&color](const Tile* tile){
        return tile != nullptr && tile->getColor().compare(color) == 0;
    });

    return result;
}

std::vector<Street*> Board::getAllStreet() const {
    std::vector<Street*> result;
    for(auto& tile: tiles){
        if(tile != nullptr){
            Street* rail = dynamic_cast<Street*>(tile);
            if(rail != nullptr) result.push_back(rail);
        }
    }

    return result;
}


std::vector<Railroad*> Board::getAllRailroad() const {
    std::vector<Railroad*> result;
    for(auto& tile: tiles){
        if(tile != nullptr){
            Railroad* rail = dynamic_cast<Railroad*>(tile);
            if(rail != nullptr) result.push_back(rail);
        }
    }

    return result;
}

std::vector<Utility*> Board::getAllUtility() const {
    std::vector<Utility*> result;
    for(auto& tile: tiles){
        if(tile != nullptr){
            Utility* util = dynamic_cast<Utility*>(tile);
            if(util != nullptr) result.push_back(util);
        }
    }

    return result;
}

void Board::addTile(Tile* newTile){
    if (newTile != nullptr && newTile->getIndex() < (int)tiles.size()) {
        tiles[newTile->getIndex()] = newTile;
    }
}

void Board::advanceFestivalEffects(Player* owner) {
    if (owner == nullptr) {
        return;
    }

    for (Tile* tile : tiles) {
        Property* property = dynamic_cast<Property*>(tile);
        if (property == nullptr || property->getOwner() != owner) {
            continue;
        }

        if (property->getFestivalMultiplier() <= 1 || property->getFestivalDuration() <= 0) {
            continue;
        }

        const int nextDuration = property->getFestivalDuration() - 1;
        property->setFestivalDuration(nextDuration);
        if (nextDuration <= 0) {
            property->setFestivalMultiplier(1);
            property->setFestivalDuration(0);
        }
    }
}

bool Board::canUpgradeProperty(Player &player, Tile* tile) {
    if(tile == nullptr) return false;
    if(tile->getColor() == "Default") return false; // asumsi namanya "Default"
    std::vector<Tile*> colorGroup = getColorGroup(tile->getColor());
    Street* targetProp = dynamic_cast<Street*>(tile);
    if(targetProp->getCurrentLevel() == 5) return false;
    bool imbalanceTileExists = std::any_of(colorGroup.begin(), colorGroup.end(), [&player, targetProp](Tile* tile){
        Street* prop = dynamic_cast<Street*>(tile);
        if(prop->getOwner() != &player) return true;
        return targetProp->getCurrentLevel() - prop->getCurrentLevel() > 0;
    });
    return !imbalanceTileExists;
}

int Board::getRailroadLevel(Tile* tile) {
    // TODO: Implement get railroad level
    if(tile == nullptr) return -1;
    Railroad* prop = dynamic_cast<Railroad*>(tile);
    if(prop == nullptr) return -1; // tile invalid
    std::vector<Railroad*> railroads = getAllRailroad();
    return std::count_if(railroads.begin(), railroads.end(), [prop](Railroad* rail){
        return rail != nullptr && rail->getOwner() == prop->getOwner();
    });
}

int Board::getUtilityLevel(Tile* tile) {
    // TODO: Implement get utility level
    if(tile == nullptr) return -1;
    Utility* prop = dynamic_cast<Utility*>(tile);
    if(prop == nullptr) return -1; // tile invalid
    std::vector<Utility*> utils = getAllUtility();
    return std::count_if(utils.begin(), utils.end(), [prop](Utility* util){
        return util != nullptr && util->getOwner() == prop->getOwner();
    });
}

Player* Board::getNextPlayer(Player* player) {
    std::vector<Player*> players = GameManager::getInstance().getPlayer();
    auto closest = std::min_element(players.begin(), players.end(), [&](const Player* a, const Player* b){
        if(a == player) return false;
        if(b == player) return false;
        int posA = a->getCurrentTile()->getIndex();
        int posB = b->getCurrentTile()->getIndex();
        int posX = player->getCurrentTile()->getIndex();
        return ((posA - posX + tiles.size()) % tiles.size()) < ((posB - posX + tiles.size()) % tiles.size());
    });
    // TODO: Implement get next player logic
    return *closest;
}

int Board::stringToIndex(const std::string& str) {
    auto it = std::find_if(tiles.begin(), tiles.end(), [&str](const Tile* tile){
        return tile != nullptr && tile->getCode() == str;
    });

    if (it == tiles.end()) {
        return -1;
    }

    return static_cast<int>(std::distance(tiles.begin(), it));
}

void Board::printBoard() const {
    GameManager::getInstance().writeLine("Board tiles (1-indexed):");
    for (size_t i = 0; i < tiles.size(); i++) {
        if (tiles[i] != nullptr) {
            GameManager::getInstance().writeLine(std::to_string(i + 1) + ": " + tiles[i]->getCode() + " - " + tiles[i]->getName());
        }
    }
}

void Board::cetakAkta(std::string code) {
    Tile* tile = getTile(code);
    if (tile == nullptr) {
        GameManager::getInstance().writeLine("Tile dengan kode " + code + " tidak ditemukan.");
        return;
    }

    Property* property = dynamic_cast<Property*>(tile);
    if (property == nullptr) {
        GameManager::getInstance().writeLine("Tile dengan kode " + code + " bukan properti.");
        return;
    }

    std::string ownerName = (property->getOwner() != nullptr) ? property->getOwner()->getUsername() : "BANK";
    GameManager::getInstance().writeLine("=== Akta Properti: " + property->getName() + " (" + property->getCode() + ") ===");
    GameManager::getInstance().writeLine("Pemilik: " + ownerName);
    GameManager::getInstance().writeLine("Harga Tanah: M" + std::to_string(property->getLandCost()));
    std::string statusLabel = "BANK";
    if (property->getPropertyStatus() == OWNED) {
        statusLabel = "OWNED";
    } else if (property->getPropertyStatus() == MORTGAGED) {
        statusLabel = "MORTGAGED";
    }
    GameManager::getInstance().writeLine("Status Properti: " + statusLabel);
    GameManager::getInstance().writeLine("Nilai Gadai: M" + std::to_string(property->getMortgageValue()));

    Street* street = dynamic_cast<Street*>(property);
    if (street != nullptr) {
        GameManager::getInstance().writeLine("Harga Sewa (L0): M" + std::to_string(street->getRentCostLevel(0)));
        GameManager::getInstance().writeLine("Harga Sewa (1 rumah): M" + std::to_string(street->getRentCostLevel(1)));
        GameManager::getInstance().writeLine("Harga Sewa (2 rumah): M" + std::to_string(street->getRentCostLevel(2)));
        GameManager::getInstance().writeLine("Harga Sewa (3 rumah): M" + std::to_string(street->getRentCostLevel(3)));
        GameManager::getInstance().writeLine("Harga Sewa (4 rumah): M" + std::to_string(street->getRentCostLevel(4)));
        GameManager::getInstance().writeLine("Harga Sewa (Hotel): M" + std::to_string(street->getRentCostLevel(5)));
        GameManager::getInstance().writeLine("Harga Rumah: M" + std::to_string(street->getHouseCost()));
        GameManager::getInstance().writeLine("Harga Hotel: M" + std::to_string(street->getHotelCost()));
        GameManager::getInstance().writeLine("Level Rumah: " + std::to_string(street->getCurrentLevel()));
        return;
    }

    Railroad* railroad = dynamic_cast<Railroad*>(property);
    if (railroad != nullptr) {
        GameManager::getInstance().writeLine("Jenis: RAILROAD");
        GameManager::getInstance().writeLine("Harga Sewa Saat Ini: M" + std::to_string(railroad->getRentCost()));
        return;
    }

    Utility* utility = dynamic_cast<Utility*>(property);
    if (utility != nullptr) {
        GameManager::getInstance().writeLine("Jenis: UTILITY");
        GameManager::getInstance().writeLine("Harga Sewa Saat Ini: M" + std::to_string(utility->getRentCost()));
    }
}