#pragma once

#include <string>
#include <vector>

class Player;
class Card;

class Tile {
protected:
    int index;
    std::string code;
    std::string color;

public:
    Tile(int, const std::string&, const std::string&);
    virtual ~Tile() = default;

    int getIndex() const;
    std::string getCode() const;
    std::string getColor() const;
    virtual void runTile(Player*) = 0;
};

enum PROPERTY_STATUS {
    BANK,
    OWNED,
    MORTGAGED
};

class Property : public Tile {
protected:
    int landCost;
    int mortgageValue;
    int festivalMultiplier;
    int festivalDuration;
    Player* owner;
    PROPERTY_STATUS propertyStatus;

public:
    Property(int, const std::string&, const std::string&, int, int);

    int getLandCost() const;
    int getMortgageValue() const;
    int getFestivalMultiplier() const;
    int getFestivalDuration() const;
    Player* getOwner() const;
    PROPERTY_STATUS getPropertyStatus() const;

    void setFestivalMultiplier(int);
    void setFestivalDuration(int);
    void setOwner(Player*);
    void setPropertyStatus(PROPERTY_STATUS);

    void runTile(Player*) override;
    virtual int getRentCost() const = 0;
};

class Railroad : public Property {
private:
    std::vector<int> rentCost;

public:
    Railroad(int, const std::string&, const std::string&, int, int, const std::vector<int>&);
    void runTile(Player*) override;
    int getRentCost() const override;
};

class Utility : public Property {
private:
    std::vector<int> costMultiplier;

public:
    Utility(int, const std::string&, const std::string&, int, int, const std::vector<int>&);
    void runTile(Player*) override;
    int getRentCost() const override;
};

class Street : public Property {
private:
    int houseCost;
    int hotelCost;
    std::vector<int> rentCost;
    int currentLevel;

public:
    Street(int, const std::string&, const std::string&, int, int, int, int, const std::vector<int>&, int);

    int getHouseCost() const;
    int getHotelCost() const;
    int getCurrentLevel() const;
    void setCurrentLevel(int);

    void runTile(Player*) override;
    int getRentCost() const override;
};

class CardTile : public Tile {
public:
    CardTile(int, const std::string&, const std::string&);
    void runTile(Player*) override;
};

class Festival : public Tile {
public:
    Festival(int, const std::string&, const std::string&);
    void addMultiplier(const std::string&);
    void runTile(Player*) override;
};

class Tax : public Tile {
public:
    Tax(int, const std::string&, const std::string&);

    void runTile(Player*) override;
    virtual void payTax(Player*) = 0;
};

class PPH : public Tax {
public:
    PPH(int, const std::string&, const std::string&);
    void payTax(Player*) override;
};

class PBM : public Tax {
private:
    int fixedTax;

public:
    PBM(int, const std::string&, const std::string&, int);
    int getFixedTax() const;
    void payTax(Player*) override;
};

class Go : public Tile {
public:
    Go(int, const std::string&, const std::string&);
    void givePayments(Player*);
    void runTile(Player*) override;
};

class Prison : public Tile {
private:
    int fee;

public:
    Prison(int, const std::string&, const std::string&, int);

    int getFee() const;
    bool checkJailed(Player*) const;
    void payFee(Player*);
    void setJailed(Player*);
    void freeFromJailed(Player*);
    void runTile(Player*) override;
};

class Trap : public Tile {
public:
    Trap(int, const std::string&, const std::string&);
    void runTile(Player*) override;
};

class FreeParking : public Tile {
public:
    FreeParking(int, const std::string&, const std::string&);
    void runTile(Player*) override;
};

