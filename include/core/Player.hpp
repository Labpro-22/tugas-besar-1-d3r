#pragma once

#include <string>
#include <utility>
#include <vector>
#include "Tile.hpp"
#include "Card.hpp"
#include "Deck.hpp"
#include "Board.hpp"
#include "../utils/GameException.hpp"

using namespace std;

class Board;

enum PLAYER_STATUS { ACTIVE, BANKRUPT, JAILED };
enum CARD_EFFECT { NOEFFECT, DISCOUNT, SHIELD };

class Player {
    private:
        string username;
        int currency;
        PLAYER_STATUS currentStatus;
        CardDeck<SkillCard> deck;
        Tile* currentTile;
        vector<Property*> ownedProperties;
        float discountValue;
        int jailTurnCount;
        bool canUseCard;
    public:
        // getter

        string getUsername() const { return username; };
        int getCurrency() const { return currency; };
        PLAYER_STATUS getStatus() const { return currentStatus; };
        const CardDeck<SkillCard>& getDeck() const { return deck; };
        Tile* getCurrentTile() const { return currentTile; }
        vector<Property*> getOwnedProperties() const { return ownedProperties; };
        float getDiscount() const { return discountValue; };
        int getJailTurn() const { return jailTurnCount; };
        bool getCanUseCard() const { return canUseCard; };

        // setter

        void setUsername(string username) { this->username = username; };
        void setCurrency(int currency) { this->currency = currency; };
        void setCurrentStatus(PLAYER_STATUS currentStatus) { this->currentStatus = currentStatus; };
        void setDeck(CardDeck<SkillCard> deck) { this->deck = std::move(deck); };
        void setCurrentTile(Tile* currentTile) { this->currentTile = currentTile; };
        void setOwnedProperties(vector<Property*> ownedProperties) { this->ownedProperties = ownedProperties; };
        void setDiscountValue(float discountValue) { this->discountValue = discountValue; };
        void setJailTurnCount(int jailTurnCount) { this->jailTurnCount = jailTurnCount; };
        void setCanUseCard(bool canUseCard) { this->canUseCard = canUseCard; };
        
        // specific method

        void buyBackMortgaged(Property* mortgaged, Board* board);
        Player* operator+=(int money); // untuk proses penambahan currency
        Player* operator-=(int money); // untuk proses pengurangan currency
        void moveTo(Tile* destination, bool getPayment);
        void mortgageProperty(Property* property, Board* board); // ubah status Tile jadi mortgaged
        void setToJailed();
        int getTotalWealth(const Board* board) const; // helper func to easily count tax
};
