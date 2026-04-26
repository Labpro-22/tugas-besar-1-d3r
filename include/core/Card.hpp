#pragma once

#include <string>
#include <vector>

class Player;

class Card {
protected:
    std::string cardName;
    std::string cardDescription;

public:
    Card(const std::string& cardName, const std::string& cardDescription);
    virtual ~Card() = default;

    std::string getCardName() const;
    std::string getCardDescription() const;
    virtual void useCard(Player*, std::vector<Player*>) = 0;
};

enum CARD_TYPE {
    AUTO_USE,
    SKILL
};

class AutoUseCard : public Card {
private:
    CARD_TYPE cardType;
public:
    AutoUseCard(const std::string& cardName, const std::string& cardDescription);
    virtual ~AutoUseCard() = default;

    CARD_TYPE getCardType() const;
    void useCard(Player*, std::vector<Player*>) override = 0;
};

class BirthDayCard : public AutoUseCard {
public:
    BirthDayCard();
    void useCard(Player*, std::vector<Player*>) override;
};

class DoctorCard : public AutoUseCard {
public:
    DoctorCard();
    void useCard(Player*, std::vector<Player*>) override;
};

class CampaignCard : public AutoUseCard {
public:
    CampaignCard();
    void useCard(Player*, std::vector<Player*>) override;
};

class NearestStationCard : public AutoUseCard {
public:
    NearestStationCard();
    void useCard(Player*, std::vector<Player*>) override;
};

class MoveBackCard : public AutoUseCard {
public:
    MoveBackCard();
    void useCard(Player*, std::vector<Player*>) override;
};

class ToJailCard : public AutoUseCard {
public:
    ToJailCard();
    void useCard(Player*, std::vector<Player*>) override;
};

class SkillCard : public Card {
protected:
    int cardValue;
    int cardDuration;
public:
    SkillCard(const std::string& cardName, const std::string& cardDescription, const int cardValue, const int cardDuration);
    virtual ~SkillCard() = default;

    CARD_TYPE getCardType() const;
    int getCardValue() const;
    int getCardDuration() const;
    void setCardValue(int value);
    void setCardDuration(int duration);
    void useCard(Player*, std::vector<Player*>) override = 0;
};

class MoveCard : public SkillCard {
public:
    MoveCard();
    MoveCard(int);
    int getTileCount() const;
    void setTileCount(int);
    void useCard(Player*, std::vector<Player*>) override;
};

class DiscountCard : public SkillCard {
public:
    DiscountCard();
    DiscountCard(int);
    float getDiscount() const;
    void setDiscount(int);
    void useCard(Player*, std::vector<Player*>) override;
};

class ShieldCard : public SkillCard {
public:
    ShieldCard();
    void useCard(Player*, std::vector<Player*>) override;
};

class TeleportCard : public SkillCard {
public:
    TeleportCard();
    void useCard(Player*, std::vector<Player*>) override;
};

class LassoCard : public SkillCard {
public:
    LassoCard();
    void useCard(Player*, std::vector<Player*>) override;
};

class DemolitionCard : public SkillCard {
public:
    DemolitionCard();
    void useCard(Player*, std::vector<Player*>) override;
};

class FreeJailCard : public SkillCard {
public:
    FreeJailCard();
    void useCard(Player*, std::vector<Player*>) override;
};
