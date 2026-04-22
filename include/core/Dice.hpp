#pragma once

#include <vector>

class Dice {
private:
    int first;
    int second;

public:
    Dice();

    void roll();
    bool setValues(int first, int second);
    bool setValues(const std::vector<int>& values);

    int getFirst() const;
    int getSecond() const;
    int getTotal() const;
    bool isDouble() const;
    std::vector<int> getValues() const;

    static bool isValidValue(int value);
};
