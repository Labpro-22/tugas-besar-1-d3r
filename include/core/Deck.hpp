#pragma once

#include <vector>

template <typename T>
class CardDeck {
private:
	std::vector<T> availableCards;
	std::vector<T> usedCard;

public:
	CardDeck() = default;
	virtual ~CardDeck() = default;

	void shuffleDeck();
	void printDeck() const;
	T topDeck();
	T* getRandomCard();
};
