#include "../../include/core/Tile.hpp"

#include "../../include/core/GameManager.hpp"

Festival::Festival(int index, const std::string& code, const std::string& name, const std::string& color) : Tile(index, code, name, color) {}
void Festival::addMultiplier(const std::string& targetColor) {
    for (Tile* tile : GameManager::getInstance().getBoard().getColorGroup(targetColor)) {
        Property* property = dynamic_cast<Property*>(tile);
        if (property != nullptr) {
            property->setFestivalMultiplier(2);
            property->setFestivalDuration(3);
        }
    }
}
void Festival::runTile(Player*) {
    const std::vector<Tile*> tiles = GameManager::getInstance().getBoard().getTiles();
    for (Tile* tile : tiles) {
        Property* property = dynamic_cast<Property*>(tile);
        if (property != nullptr && property->getColor() != "DEFAULT") {
            addMultiplier(property->getColor());
            return;
        }
    }
}

