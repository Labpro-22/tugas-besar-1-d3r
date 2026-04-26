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
enum MOVE_DIRECTION { FORWARD, BACKWARD };

class Player {
    private:
        string username;
        int currency;
        PLAYER_STATUS currentStatus;
        CardDeck<SkillCard> deck;
        Tile* currentTile;
        CARD_EFFECT activeCardEffect;
        float discountValue;
        int effectTurns;
        int jailTurnCount;
        int doubleCount;
        bool canUseCard;
        void moveOneStep(const Board& board, bool getPayment, MOVE_DIRECTION direction);
    public:
        Player();

        // getter

        string getUsername() const { return username; };
        int getCurrency() const { return currency; };
        PLAYER_STATUS getStatus() const { return currentStatus; };
        const CardDeck<SkillCard>& getDeck() const { return deck; };
        Tile* getCurrentTile() const { return currentTile; }
        CARD_EFFECT getActiveCardEffect() const { return activeCardEffect; };
        float getDiscount() const { return activeCardEffect == DISCOUNT ? discountValue : 0.0f; };
        int getDiscountTurns() const { return activeCardEffect == DISCOUNT ? effectTurns : 0; };
        int getShieldTurns() const { return activeCardEffect == SHIELD ? effectTurns : 0; };
        bool hasShield() const { return activeCardEffect == SHIELD && effectTurns > 0; };
        bool hasDiscount() const { return activeCardEffect == DISCOUNT && discountValue > 0.0f && effectTurns > 0; };
        int getJailTurn() const { return jailTurnCount; };
        int getDoubleCount() const { return doubleCount; };
        bool getCanUseCard() const { return canUseCard; };

        // setter

        void setUsername(string username) { this->username = username; };
        void setCurrency(int currency) { this->currency = currency; };
        void setCurrentStatus(PLAYER_STATUS currentStatus) { this->currentStatus = currentStatus; };
        void setDeck(CardDeck<SkillCard> deck) { this->deck = std::move(deck); };
        void setCurrentTile(Tile* currentTile) { this->currentTile = currentTile; };
        void setActiveCardEffect(CARD_EFFECT activeCardEffect) { this->activeCardEffect = activeCardEffect; };
        void setDiscountValue(float discountValue) { this->discountValue = discountValue; if (discountValue > 0.0f) this->activeCardEffect = DISCOUNT; };
        void setDiscountTurns(int discountTurns) { this->effectTurns = discountTurns; this->activeCardEffect = discountTurns > 0 ? DISCOUNT : NOEFFECT; };
        void setShieldTurns(int shieldTurns) { this->effectTurns = shieldTurns; this->activeCardEffect = shieldTurns > 0 ? SHIELD : NOEFFECT; };
        void setJailTurnCount(int jailTurnCount) { this->jailTurnCount = jailTurnCount; };
        void setDoubleCount(int doubleCount) { this->doubleCount = doubleCount; };
        void setCanUseCard(bool canUseCard) { this->canUseCard = canUseCard; };
        
        // specific method

        void buyBackMortgaged(Property* mortgaged);
        Player* operator+=(int money); // untuk proses penambahan currency
        Player* operator-=(int money); // untuk proses pengurangan currency
        void activateDiscount(float discount, int turns = 1);
        void activateShield(int turns = 1);
        void resetCardUse();
        void endTurnEffects();
        void pay(int amount, Player* creditor = nullptr);
        bool addSkillCard(SkillCard* card);
        SkillCard* removeSkillCard(int index);
        void printSkillCards() const;
        void printProperties() const;
        void moveTo(Tile* destination, bool getPayment, MOVE_DIRECTION direction = FORWARD);
        void mortgageProperty(Property* property, Board* board); // ubah status Tile jadi mortgaged
        void setToJailed();
        int getTotalWealth(const Board* board) const; // helper func to easily count tax
        int getMaxLiquidatableValue(const Board* board) const ;
        vector<Property*> getOwnedProperties() const;

};
