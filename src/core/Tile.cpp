#include "../../include/core/GameManager.hpp"
#include "../../include/core/Tile.hpp"
#include "../../include/core/Player.hpp"

// ============== GUI Property Base Class ======
void Street::callViewer(PropertyRenderCall& other) {
    other.render(this);
}
void Utility::callViewer(PropertyRenderCall& other) {
    other.render(this);
}
void Railroad::callViewer(PropertyRenderCall& other) {
    other.render(this);
}

void Street::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}
void Utility::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}
void Railroad::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}
void PBM::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}
void PPH::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}
void CardTile::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}
void Festival::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}
void Go::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}
void Prison::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}
void Trap::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}
void FreeParking::callPopUp(TilePopUpCall& other){
    other.renderPopUp(this);
}


// ============== Tile Base Class ==============
Tile::Tile(int index, const std::string& code, const std::string& name, const std::string& color)
    : index(index), code(code), name(name), color(color) {}


// ============== Property Class ==============
Property::Property(int index, const std::string& code, const std::string& name, const std::string& color, 
                   int landCost, int mortgageValue, int festivalMultiplier, 
                   int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus)
    : Tile(index, code, name, color), landCost(landCost), mortgageValue(mortgageValue),
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

int Property::getRentCostLevel(int) const {
    return getRentCost();
}

int Property::getAssetValue() const {
    return landCost;
}

// ============== Tax Class ==============
Tax::Tax(int index, const std::string& code, const std::string& name, const std::string& color)
    : Tile(index, code, name, color) {}

void Tax::runTile(Player* player) {
    if (player != nullptr) {
        payTax(player);
        
    }
}

CardTile::CardTile(int index, const std::string& code, const std::string& name, const std::string& color)
    : Tile(index, code, name, color) {}

void CardTile::runTile(Player* player) {
    if (player == nullptr) {
        return;
    }

    GameManager& game = GameManager::getInstance();
    AutoUseCard* card = nullptr;

    // seems like its save to be hardcoded...
    if (getCode() == "KSP") {
        card = game.getChanceDeck().getRandomCard();
    } else if (getCode() == "DNU") {
        card = game.getCurrencyDeck().getRandomCard();
    }

    if (card != nullptr) {
        card->useCard(player, game.getPlayers());
    }
}

// ============== Go Class ==============
Go::Go(int index, const std::string& code, const std::string& name, const std::string& color, int payment)
    : Tile(index, code, name, color), payment(payment) {}

void Go::givePayments(Player* player) {
    if (player != nullptr) {
        *player += payment;
    }
}

void Go::runTile(Player* player) {
    givePayments(player);
}


// ============== Prison Class ==============
Prison::Prison(int index, const std::string& code, const std::string& name, const std::string& color, int fee)
    : Tile(index, code, name, color), fee(fee) {}

int Prison::getFee() const { return fee; }

bool Prison::checkJailed(Player* player) const {
    if (player != nullptr) {
        return player->getStatus() == JAILED;
    }
    return false;
}

void Prison::payFee(Player* player) {
    if (player != nullptr) {
        GameManager::getInstance().pay(player, fee, nullptr);
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
        player->setJailTurnCount(0);
    }
}

void Prison::runTile(Player* player) {
    if (player != nullptr && checkJailed(player) && player->getJailTurn() > 3) {
        payFee(player);
        freeFromJailed(player);
    }
}

Trap::Trap(int index, const std::string& code, const std::string& name, const std::string& color)
: Tile(index, code, name, color){}
void Trap::runTile(Player* player){
    if (player == nullptr) {
        return;
    }
    Tile* prison = GameManager::getInstance().getBoard().getJailTile();
    if (prison == nullptr) {
        return;
    }
    player->setToJailed();
    player->moveTo(prison, false, FORWARD);
}

FreeParking::FreeParking(int index, const std::string& code, const std::string& name, const std::string& color)
: Tile(index, code, name, color){}
void FreeParking::runTile(Player*){
}
