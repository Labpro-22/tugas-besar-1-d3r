#pragma once

#include <string>
#include <vector>

class Player;
class Card;

class Tile {
protected:
    int index;
    std::string code;
    std::string name;
    std::string color;

public:
    Tile(int index, const std::string& code, const std::string& name, const std::string& color);
    virtual ~Tile() = default;

    int getIndex() const { return index; }
    std::string getCode() const { return code; }
    std::string getName() const { return name; }
    std::string getColor() const { return color; }
    virtual void runTile(Player*) = 0;
    virtual int getAssetValue() const { return 0; }    
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
    Property(int index, const std::string& code, const std::string& name, const std::string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus);

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
    int getAssetValue() const override;

};

class Railroad : public Property {
private:
    std::vector<int> rentCost;

public:
    Railroad(int index, const std::string& code, 
        const std::string& name, const std::string& color, 
        int landCost, int mortgageValue, int festivalMultiplier, 
        int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus,
        const std::vector<int>& rentCost);
    void runTile(Player*) override;
    int getRentCost() const override;
};

class Utility : public Property {
private:
    std::vector<int> costMultiplier;

public:
    Utility(int index, const std::string& code, const std::string& name, const std::string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus, const std::vector<int>& costMultiplier);
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
    Street(int index, const std::string& code, const std::string& name, const std::string& color, int landCost, int mortgageValue, int festivalMultiplier, int festivalDuration, Player* owner, PROPERTY_STATUS propertyStatus, int houseCost, int hotelCost, std::vector<int> rentCost, int currentLevel);

    int getHouseCost() const;
    int getHotelCost() const;
    int getCurrentLevel() const;
    void setCurrentLevel(int currentLevel);

    void runTile(Player*) override;
    int getRentCost() const override;
    int getAssetValue() const override;    
};

class CardTile : public Tile {
public:
    CardTile(int, const std::string&, const std::string& name, const std::string&);
    void runTile(Player*) override;
};

class Festival : public Tile {
public:
    Festival(int index, const std::string& code, const std::string& name, const std::string& color);
    void addMultiplier(const std::string&);
    void runTile(Player*) override;
};

class Tax : public Tile {
public:
    Tax(int, const std::string&, const std::string& name, const std::string&);

    void runTile(Player*) override;
    virtual void payTax(Player* player) = 0;
};

enum PPH_OPTION {
    FLAT,
    PERCENTAGE
};
class PPH : public Tax {
private:
    int flatTax;
    int taxPercentage;
public:
    PPH(int, const std::string&, const std::string&, int flatTax, int taxPercentage);
    void payTax(Player* player) override;
    void payPphTax(Player* player, PPH_OPTION option);

    int getFlatTax() const { return flatTax; };
    int getTaxPercentage() const { return taxPercentage; };
    void setFlatTax(int flatTax) { this->flatTax = flatTax; };
    void setTaxPercentage(int taxPercentage) { this->taxPercentage = taxPercentage; };

    int calculateFlatTax() const;
    int calculatePercentageTax(const Player& player) const;
    
};

class PBM : public Tax {
private:
    int fixedTax;

public:
    PBM(int, const std::string&, const std::string& name, const std::string&, int fixedTax);
    int getFixedTax() const;
    void setFixedTax(int fixedTax) { this->fixedTax = fixedTax; };

    void payTax(Player* player) override;
};

class Go : public Tile {
private:
    int payment;
public:
    Go(int, const std::string&, const std::string& name, const std::string&, int payment);
    void givePayments(Player*);
    void runTile(Player*) override;
};

class Prison : public Tile {
private:
    int fee;

public:
    Prison(int, const std::string&, const std::string& name, const std::string&, int);

    int getFee() const;
    bool checkJailed(Player*) const;
    void payFee(Player*);
    void setJailed(Player*);
    void freeFromJailed(Player*);
    void runTile(Player*) override;
};

class Trap : public Tile {
public:
    Trap(int, const std::string&, const std::string& name, const std::string&);
    void runTile(Player* player) override;
};

class FreeParking : public Tile {
public:
    FreeParking(int index, const std::string& code, const std::string& name, const std::string& color);
    void runTile(Player* player) override;
};

