#pragma once

#include <exception>
#include <string>

using namespace std;

class NimonspoliException : public exception {
    private:
        int errorCode;
        string errorMessage;
    public:
        NimonspoliException(int code, string message) : errorCode(code), errorMessage(message) {}
        int getErrorCode() const noexcept { return errorCode; }
        string getErrorMessage() const noexcept { return errorMessage; }
        virtual const char* what() const noexcept override { return errorMessage.c_str(); }
};

class InvalidDiceException : public NimonspoliException { 
    private:
        int dice1Value;
        int dice2Value;
    public:
        InvalidDiceException(int dice1, int dice2) : NimonspoliException(1, "DICE_VALUE_INVALID"), dice1Value(dice1), dice2Value(dice2) {}
        int getDice1() const noexcept { return dice1Value; }
        int getDice2() const noexcept { return dice2Value; }
};

class InvalidTileCodeException : public NimonspoliException { 
    private:
        string tileCode;
    public:
        InvalidTileCodeException(string tileCode) : NimonspoliException(2, "TILE_CODE_INVALID"), tileCode(tileCode) {}
        string getTileCode() const noexcept { return tileCode; }
};

class NoPropertyException : public NimonspoliException { 
    public:
        NoPropertyException() : NimonspoliException(3, "NO_PROPERTY_ERROR") {}
};

class NotEnoughMoneyException : public NimonspoliException { 
    private:
        string failedToPurchase;
        int moneyNeeded;
        int moneyAvailable;
    public:
        NotEnoughMoneyException(string purchaseWhat, int need, int avail) : NimonspoliException(4, "NOT_ENOUGH_MONEY"), failedToPurchase(purchaseWhat), moneyNeeded(need), moneyAvailable(avail) {}
        string purchaseWhat() const noexcept { return failedToPurchase; }
        int getMoneyNeeded() const noexcept { return moneyNeeded; }
        int getMoneyAvailable() const noexcept { return moneyAvailable; }
};

class AbilityUsedException : public NimonspoliException { 
    public:
        AbilityUsedException() : NimonspoliException(5, "SKILLCARD_ALREADY_USED") {}
};

class AbilityExceededException : public NimonspoliException { 
    public:
        AbilityExceededException() : NimonspoliException(6, "SKILLCARD_EXCEEDED_CAP") {}
};

class NoPropertyToMortgageException : public NimonspoliException { 
    public:
        NoPropertyToMortgageException() : NimonspoliException(7, "NO_PROPERTY_TO_MORTGAGE") {}
};

class FailedMortgageException : public NimonspoliException { 
    private:
        string propertyName;
        string colorGroup;
    public:
        FailedMortgageException(string prop, string color) : NimonspoliException(8, "MORTGAGE_FAILED"), propertyName(prop), colorGroup(color) {}
        string getPropertyName() const noexcept { return propertyName; }
        string getColorGroup() const noexcept { return colorGroup; }
};

class NoMortgageException : public NimonspoliException { 
    public:
        NoMortgageException() : NimonspoliException(9, "NO_MORTGAGED_PROPERTY") {}
};

class InvalidChoiceException : public NimonspoliException { 
    private:
        int minExpectedChoice;
        int maxExpectedChoice;
        int actualChoice;
    public:
        InvalidChoiceException(int selected, int min, int max) : 
            NimonspoliException(10, "INVALID_CHOICE"), minExpectedChoice(min), maxExpectedChoice(max), actualChoice(selected) {}
        int getMinChoice() const noexcept { return minExpectedChoice; }
        int getMaxChoice() const noexcept { return maxExpectedChoice; }
        int getActualChoice() const noexcept { return actualChoice; }      
};

class SaveFailedException : public NimonspoliException { 
    public:
        SaveFailedException() : NimonspoliException(11, "SAVE_FAILED") {}
};

class LoadFailedException : public NimonspoliException { 
    public:
        LoadFailedException() : NimonspoliException(12, "LOAD_FAILED") {}
};

class FileExistsException : public NimonspoliException {
    private:
        string& fileName;
    public:
        FileExistsException(string& fileName) : NimonspoliException(13, "FILE_EXISTS"), fileName(fileName)  {}
        string& getFileName() const { return fileName; }
};

class FileNotExistsException : public NimonspoliException {
    private:
        string& fileName;
    public:
        FileNotExistsException(string& fileName) : NimonspoliException(14, "FILE_NOT_EXISTS"), fileName(fileName)  {}
        string& getFileName() const { return fileName; }
};

class SaveProhibitedException : public NimonspoliException { 
    public:
        SaveProhibitedException() : NimonspoliException(15, "SAVE_PROHIBITED") {}
};