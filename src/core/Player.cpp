#include "../../include/core/Player.hpp"
#include "../../include/core/GameManager.hpp"
#include "../../include/core/Tile.hpp"

Player* Player::operator+=(int money) {
    this->currency += money;
    return this;
}

Player* Player::operator-=(int money) {
    this->currency -= money;
    if (this->currency < 0) {
        this->currentStatus = BANKRUPT;
    }
    return this;
}

void Player::moveTo(Tile* destination, bool getPayment) {
    if (destination != nullptr) {
        this->currentTile = destination;
        if (getPayment) {
            // TODO: Handle payment when landing on tile GO or passing tile GO
            if(destination->getIndex() < this->currentTile->getIndex()){
                Tile* Go = GameManager::getInstance().getBoard().getTile("GO");
                Go->runTile(this);
            }
        }
    }
}

void Player::mortgageProperty(Tile* property) {
    // TODO: Implement mortgage property logic
}

void Player::setToJailed() {
    this->currentStatus = JAILED;
    this->jailTurnCount = 3;  // Default 3 turns in jail
}

void Player::buyBackMortgaged(Tile* mortgaged) {
    // TODO: Implement buy back mortgaged property logic
}

int Player::getTotalWealth(const Board* board) const {
    int total = currency;

    if (board == nullptr){
        return total;
    }

    for(Tile* tile : board->getTiles()){
        Property* prop = dynamic_cast<Property*>(tile);
        if (prop == nullptr){
            continue;
        }
        if(prop->getOwner() != this){
            continue;
        }
        total += prop->getAssetValue();
    }
    return total;
}

