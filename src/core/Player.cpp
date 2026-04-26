#include "../../include/core/Player.hpp"
#include "../../include/core/GameManager.hpp"
#include "../../include/core/Logger.hpp"
#include "../../include/core/Tile.hpp"
#include <algorithm>
#include <map>

Player::Player()
    : username(""), currency(0), currentStatus(ACTIVE), currentTile(nullptr),
    activeCardEffect(NOEFFECT), discountValue(0.0f), effectTurns(0), jailTurnCount(0),
      canUseCard(true) {}

Player* Player::operator+=(int money) {
    this->currency += money;
    return this;
}

Player* Player::operator-=(int money) {
    this->currency -= money;
    return this;
}

void Player::activateDiscount(float discount, int turns) {
    this->activeCardEffect = DISCOUNT;
    this->discountValue = discount;
    this->effectTurns = turns;
}

void Player::activateShield(int turns) {
    this->activeCardEffect = SHIELD;
    this->discountValue = 0.0f;
    this->effectTurns = turns;
}

void Player::resetCardUse() {
    this->canUseCard = true;
}

void Player::endTurnEffects() {
    if (effectTurns > 0) {
        effectTurns--;
        if (effectTurns == 0) {
            activeCardEffect = NOEFFECT;
            discountValue = 0.0f;
        }
    }
}

bool Player::addSkillCard(SkillCard* card) {
    Logger &logger = Logger::getInstance();
    if (card == nullptr) {
        return false;
    }

    deck.addCard(card);

    try {
        if (deck.size() > 3) {
            throw AbilityExceededException();
        }
    } catch (const AbilityExceededException&) {
        printSkillCards();

        GameManager::getInstance().writeLine(username + " mendapat kartu kemampuan: " + card->getCardName());
        GameManager::getInstance().writeLine("Deck penuh, buang kartu!");
        int cardNumber = GameManager::getInstance().getCommandHandler().askInt("Buang Kartu (1-" + std::to_string(deck.size()) + "): ", 1, deck.size());

        SkillCard* discardedCard = deck.removeAt(cardNumber - 1);
        logger.log(username, StateLog::GET_CARD, "Mendapatkan kartu " + card->getCardName() + " dan membuang kartu " + discardedCard->getCardName());
        if (discardedCard != nullptr) {
            delete discardedCard;
        }
        return true;
    }

    GameManager::getInstance().writeLine(username + " mendapat kartu kemampuan: " + card->getCardName());
    logger.log(username, StateLog::GET_CARD, "Mendapatkan kartu " + card->getCardName());
    return true;
}

SkillCard* Player::removeSkillCard(int index) {
    if (index < 0) {
        return nullptr;
    }

    return deck.removeAt(index);
}

void Player::printSkillCards() const {
    const std::vector<SkillCard*>& cards = deck.getCards();
    if (cards.empty()) {
        GameManager::getInstance().writeLine("Tidak ada kartu kemampuan.");
        return;
    }

    for (size_t i = 0; i < cards.size(); i++) {
        if (cards[i] != nullptr) {
            GameManager::getInstance().writeLine(std::to_string(i + 1) + ". " + cards[i]->getCardName() + " - " + cards[i]->getCardDescription());
        }
    }
}

void Player::printProperties() const {
    GameManager& game = GameManager::getInstance();

    std::vector<Property*> owned;
    for (Tile* tile : game.getBoard().getTiles()) {
        Property* property = dynamic_cast<Property*>(tile);
        if (property != nullptr && property->getOwner() == this) {
            owned.push_back(property);
        }
    }

    if (owned.empty()) {
        throw NoPropertyException();
    }

    auto formatColorLabel = [](std::string raw) {
        std::replace(raw.begin(), raw.end(), '_', ' ');
        return raw;
    };

    auto groupNameFor = [&formatColorLabel](Property* property) {
        if (dynamic_cast<Railroad*>(property) != nullptr) {
            return std::string("STASIUN");
        }
        if (dynamic_cast<Utility*>(property) != nullptr) {
            return std::string("UTILITAS");
        }
        return formatColorLabel(property->getColor());
    };

    std::map<std::string, std::vector<Property*>> grouped;
    for (Property* property : owned) {
        grouped[groupNameFor(property)].push_back(property);
    }

    game.writeLine("=== Properti Milik: " + username + " ===");
    game.writeLine("");

    int totalAsset = 0;
    for (const auto& section : grouped) {
        game.writeLine("[" + section.first + "]");

        for (Property* property : section.second) {
            std::string row = "  - " + property->getName() + " (" + property->getCode() + ")";

            Street* street = dynamic_cast<Street*>(property);
            if (street != nullptr) {
                if (street->getCurrentLevel() >= 1 && street->getCurrentLevel() <= 4) {
                    row += "\t" + std::to_string(street->getCurrentLevel()) + " rumah";
                } else if (street->getCurrentLevel() == 5) {
                    row += "\tHotel";
                }
            }

            row += "\tM" + std::to_string(property->getLandCost());
            if (property->getPropertyStatus() == MORTGAGED) {
                row += "\tMORTGAGED [M]";
            } else {
                row += "\tOWNED";
            }

            game.writeLine(row);
            totalAsset += property->getAssetValue();
        }

        game.writeLine("");
    }

    game.writeLine("Total kekayaan properti: M" + std::to_string(totalAsset));
}

void Player::moveTo(Tile* destination, bool getPayment) {
    if (destination != nullptr) {
        Tile* previousTile = this->currentTile;
        this->currentTile = destination;
        if (getPayment && previousTile != nullptr && destination->getIndex() < previousTile->getIndex()) {
            Tile* go = GameManager::getInstance().getBoard().getTile("GO");
            if (go != nullptr && go != destination) {
                go->runTile(this);
            }
        }

        destination->runTile(this);
    }
}

void Player::mortgageProperty(Property* property, Board* board) {
    // Check if property actually points to a property
    if(property == nullptr) throw NoPropertyToMortgageException();

    // Check if the property is owned by the player and not mortgaged
    if(property->getOwner() != this) throw NoPropertyToMortgageException();
    if(property->getPropertyStatus() != OWNED) throw NoPropertyToMortgageException();

    // Check if there are buildings exist in the property's color group
    vector<Tile*> colorGroupProperties = board->getColorGroup(property->getColor()); 
    for_each(colorGroupProperties.begin(), colorGroupProperties.end(), [&property, this] (Tile* tile) {
        Property* owned = dynamic_cast<Property*>(tile);
        if(owned == nullptr || owned->getOwner() != this) return;
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
    Logger& logger = Logger::getInstance();
    logger.log(username, StateLog::GO_JAIL, "Masuk ke penjara");
    this->currentStatus = JAILED;
    this->jailTurnCount = 3;  // Default 3 turns in jail
}

void Player::buyBackMortgaged(Property* mortgaged) {
    // Check if mortgaged actually points to a property
    if(mortgaged == nullptr) throw NoMortgageException();

    // Check if mortgaged property is owned by the player and is currently mortgaged
    if(mortgaged->getOwner() != this) throw NoMortgageException();
    if(mortgaged->getPropertyStatus() != MORTGAGED) throw NoMortgageException();

    if(this->currency < mortgaged->getLandCost()) throw NotEnoughMoneyException("menebus " + mortgaged->getName(), mortgaged->getLandCost(), this->currency);

    *this -= mortgaged->getLandCost();
    mortgaged->setPropertyStatus(OWNED);
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

int Player::getMaxLiquidatableValue(const Board* board) const  {
    int maxCash = this->currency;
    for(Tile* tile : board->getTiles()){
        Property* prop = dynamic_cast<Property*>(tile);
        if (prop != nullptr && prop->getOwner() == this){
            if (prop->getPropertyStatus() == OWNED){
                int propVal = prop->getLandCost();

                Street* street = dynamic_cast<Street*>(prop);
                if (street != nullptr){
                    int buildingVal = street->getBuildingValue();
                    propVal += (buildingVal / 2);
                }

                maxCash += propVal;
            }
        }
    }
    return maxCash;
}

vector<Property*> Player::getOwnedProperties() const {
    GameManager& gm = GameManager::getInstance();
    vector<Tile*> tiles = gm.getBoard().getTiles();
    vector<Property*> props;
    for(Tile* tile : tiles){
        Property* propTiles = dynamic_cast<Property*>(tile);
        if (propTiles && propTiles->getOwner() == this){
            props.push_back(propTiles);
        }
    }
    return props;
}
