#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"
#include "../../include/core/GameManager.hpp"

Festival::Festival(int index, const std::string& code, const std::string& color) : Tile(index, code, color) {}
void Festival::addMultiplier(Tile* tile) {
    if (tile ==nullptr){
        return;
    }
    Property* prop = dynamic_cast<Property*>(tile);

    if (prop == nullptr) {
        return;
    }

    // multiplier can be multiplied by * 2 (max 3 times)
    // so 1 (base) * 2 * 2 * 2 = 8 (max) 

    int multiplier = prop->getFestivalMultiplier();
    if (multiplier < 8) {
        multiplier *= 2;

        prop->setFestivalMultiplier(multiplier);
    } else {
        multiplier = 8;
    }
    

    prop->setFestivalDuration(3);
}

void Festival::runTile(Player* player) {
    if (player == nullptr){
        return;
    }
    
    vector<Tile*> potentialFestival;
    for(Tile* tile : GameManager::getInstance().getBoard().getTiles()){
        Property* prop = dynamic_cast<Property*>(tile);
        if(prop == nullptr){
            return;
        }
        if(prop->getOwner() != player){
            return;
        }
        potentialFestival.push_back(tile);
    }

    if (potentialFestival.empty()){
        return;
    }

    // again, need some kind follow up input on this so it will be unfinished
    // in the mean time:
    // cin >> choice;
    // if (choice < 1 || choice > static_cast<int>(potentialFestival.size())){
    //      return;
    //}
    // addMultiplier(potentialFestival[choice-1]);
}

