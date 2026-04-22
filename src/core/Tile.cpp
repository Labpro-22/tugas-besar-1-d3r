#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"

// ============== Tile Base Class ==============
Tile::Tile(int index, const std::string& code, const std::string& color)
    : index(index), code(code), color(color) {}


// ============== Property Class ==============
Property::Property(int index, const std::string& code, const std::string& color, 
                   int landCost, int mortgageValue, int festivalMultiplier, 
                   int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus)
    : Tile(index, code, color), landCost(landCost), mortgageValue(mortgageValue),
      festivalMultiplier(festivalMultiplier), festivalDuration(festivalDuration),
      owner(owner), propertyStatus(propertyStatus) {}

int Property::getLandCost() const { return landCost; }
int Property::getMortgageValue() const { return mortgageValue; }
int Property::getFestivalMultiplier() const { return festivalMultiplier; }
int Property::getFestivalDuration() const { return festivalDuration; }
Player* Property::getOwner() const { return owner; }
PROPERTY_STATUS Property::getPropertyStatus() const { return propertyStatus; }

void Property::setFestivalMultiplier(int multiplier) { this->festivalMultiplier = multiplier; }
void Property::setFestivalDuration(int duration) { this->festivalDuration = duration; }
void Property::setOwner(Player* newOwner) { this->owner = newOwner; }
void Property::setPropertyStatus(PROPERTY_STATUS status) { this->propertyStatus = status; }

void Property::runTile(Player* player) {
    // TODO: Implement property tile behavior
}


// ============== Tax Class ==============
Tax::Tax(int index, const std::string& code, const std::string& color)
    : Tile(index, code, color) {}

void Tax::runTile(Player* player) {
    if (player != nullptr) {
        payTax(player);
    }
}


// ============== Go Class ==============
Go::Go(int index, const std::string& code, const std::string& color, int payment)
    : Tile(index, code, color), payment(payment) {}

void Go::givePayments(Player* player) {
    if (player != nullptr) {
        *player += payment;
    }
}

void Go::runTile(Player* player) {
    givePayments(player);
}


// ============== Prison Class ==============
Prison::Prison(int index, const std::string& code, const std::string& color, int fee)
    : Tile(index, code, color), fee(fee) {}

int Prison::getFee() const { return fee; }

bool Prison::checkJailed(Player* player) const {
    if (player != nullptr) {
        return player->getStatus() == JAILED;
    }
    return false;
}

void Prison::payFee(Player* player) {
    if (player != nullptr) {
        *player -= fee;
    }
}

void Prison::setJailed(Player* player) {
    if (player != nullptr) {
        player->setToJailed();
    }
}

void Prison::freeFromJailed(Player* player) {
    if (player != nullptr) {
        player->setCurrentStatus(ACTIVE);
    }
}

void Prison::runTile(Player* player) {
    if (player != nullptr && checkJailed(player)) {
        payFee(player);
    }
}
