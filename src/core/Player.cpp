#include "../../include/core/Player.hpp"
#include "../../include/core/Tile.hpp"
#include <algorithm>

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

void Player::mortgageProperty(Property* property, Board* board) {
    // Check if property actually points to a property
    if(property == nullptr) return;

    // Check if the property is owned by the player and not mortgaged
    if(property->getOwner() != this) return;
    if(property->getPropertyStatus() != OWNED) return;

    // Check if there are buildings exist in the property's color group
    vector<Tile*> colorGroupProperties = board->getColorGroup(property->getColor()); 
    for_each(colorGroupProperties.begin(), colorGroupProperties.end(), [&property, this] (Property* owned) {
        if(owned->getOwner() != this) return;
        Street* streetOwned = dynamic_cast<Street*>(owned);
        if(streetOwned != nullptr) {
            if(property->getColor() == streetOwned->getColor() && streetOwned->getCurrentLevel() > 0) {
                throw FailedMortgageException(property->getName(), property->getColor());
            }
        }
    });

    property->setPropertyStatus(MORTGAGED);
    *this += property->getMortgageValue();
}

void Player::setToJailed() {
    this->currentStatus = JAILED;
    this->jailTurnCount = 3;  // Default 3 turns in jail
}

void Player::buyBackMortgaged(Property* mortgaged, Board* board) {
    // Check if mortgaged actually points to a property
    if(mortgaged == nullptr) return;

    // Check if mortgaged property is owned by the player and is currently mortgaged
    if(mortgaged->getOwner() != this) return;
    if(mortgaged->getPropertyStatus() != MORTGAGED) return;

    if(this->currency < mortgaged->getLandCost()) throw NotEnoughMoneyException("menebus " + mortgaged->getName(), mortgaged->getLandCost(), this->currency);

    *this -= mortgaged->getLandCost();
    mortgaged->setPropertyStatus(OWNED);
}
