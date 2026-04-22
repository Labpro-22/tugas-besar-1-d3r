#include "../../include/core/Board.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include "../../include/core/GameManager.hpp"
#include "../../include/core/Tile.hpp"
using namespace std;

Board::Board(int size): tiles(size, nullptr) {
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
    return tiles.at(index);
}

Tile* Board::getJailTile() const {
    return getTile("PEN");
}

Tile* Board::goToTile(Tile& current, int moveAmount) const {
    auto it = std::find_if(tiles.begin(), tiles.end(), [&current](const Tile* tile){
        return tile != nullptr && tile->getCode().compare(current.getCode()) == 0;
    });
    if (it != tiles.end()) {
        size_t index = distance(tiles.begin(), it);
        index = (index + moveAmount) % tiles.size();
        return tiles[index];
    } else {
        return nullptr;
    }
}

std::vector<Tile*> Board::getColorGroup(const std::string& color) const {
    std::vector<Tile*> result;
    std::copy_if(tiles.begin(), tiles.end(), std::back_inserter(result), [&color](const Tile* tile){
        return tile != nullptr && tile->getColor().compare(color) == 0;
    });

    return result;
}

void Board::addTile(Tile* newTile){
    if (newTile != nullptr && newTile->getIndex() < (int)tiles.size()) {
        tiles[newTile->getIndex()] = newTile;
    }
}

bool Board::canBuildHouse(Player player, Tile* tile) {
    // TODO: Implement check for building house
    return false;
}

int Board::getRailroadLevel(Tile* tile) {
    // TODO: Implement get railroad level
    return 0;
}

int Board::getUtilityLevel(Tile* tile) {
    // TODO: Implement get utility level
    return 0;
}

Player* Board::getNextPlayer(int currentIndex) {
    // TODO: Implement get next player logic
    return nullptr;
}

int Board::stringToIndex(const std::string& str) {
    // TODO: Implement string to index conversion
    return 0;
}

Tile* Board::goToTile(const std::string& code, int moveAmount) {
    Tile* currentTile = getTile(code);
    if (currentTile != nullptr) {
        return goToTile(*currentTile, moveAmount);
    }
    return nullptr;
}