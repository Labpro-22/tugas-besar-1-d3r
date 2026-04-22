#include "../../include/core/Player.hpp"
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
            // TODO: Handle payment when landing on tile
            destination->runTile(this);
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
