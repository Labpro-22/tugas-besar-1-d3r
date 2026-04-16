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
public:
    SkillCard(const std::string& cardName, const std::string& cardDescription);
    virtual ~SkillCard() = default;

    CARD_TYPE getCardType() const;
    void useCard(Player*, std::vector<Player*>) override = 0;
};

class MoveCard : public SkillCard {
private:
    int moveTileMax;

public:
    MoveCard(int);
    int getMoveTileMax() const;
    void setMoveTileMax(int);
    void useCard(Player*, std::vector<Player*>) override;
};

class DiscountCard : public SkillCard {
private:
    float discount;
public:
    DiscountCard(float);
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
