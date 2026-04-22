#include "../../include/core/Dice.hpp"

#include <random>

Dice::Dice() : first(0), second(0) {}

void Dice::roll() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(1, 6);

    first = distribution(gen);
    second = distribution(gen);
}

bool Dice::setValues(int first, int second) {
    if (!isValidValue(first) || !isValidValue(second)) {
        return false;
    }

    this->first = first;
    this->second = second;
    return true;
}

bool Dice::setValues(const std::vector<int>& values) {
    if (values.size() < 2) {
        return false;
    }

    return setValues(values[0], values[1]);
}

int Dice::getFirst() const {
    return first;
}

int Dice::getSecond() const {
    return second;
}

int Dice::getTotal() const {
    return first + second;
}

bool Dice::isDouble() const {
    return first == second;
}

std::vector<int> Dice::getValues() const {
    return {first, second};
}

bool Dice::isValidValue(int value) {
    return value >= 1 && value <= 6;
}
