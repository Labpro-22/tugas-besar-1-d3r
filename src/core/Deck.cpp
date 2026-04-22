#include "../../include/core/Deck.hpp"

#include <algorithm>
#include <iostream>
#include <random>

#include "../../include/core/Card.hpp"

template <typename T>
void CardDeck<T>::addCard(T* card) {
	if (card != nullptr) {
		availableCards.push_back(card);
	}
}

template <typename T>
void CardDeck<T>::shuffleDeck() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(availableCards.begin(), availableCards.end(), gen);
}

template <typename T>
void CardDeck<T>::printDeck() const {
	for (const T* card : availableCards) {
		if (card != nullptr) {
			std::cout << card->getCardName() << "\n";
		}
	}
}

template <typename T>
T* CardDeck<T>::topDeck() {
	if (availableCards.empty()) {
		return nullptr;
	}

	T* topCard = availableCards.back();
	availableCards.pop_back();
	usedCard.push_back(topCard);
	return topCard;
}

template <typename T>
T* CardDeck<T>::getRandomCard() {
	if (availableCards.empty()) {
		if (usedCard.empty()) {
			return nullptr;
		}

		availableCards = usedCard;
		usedCard.clear();
		shuffleDeck();
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> dis(0, availableCards.size() - 1);
	size_t selectedIndex = dis(gen);

	T* selectedCard = availableCards[selectedIndex];
	availableCards.erase(availableCards.begin() + static_cast<long long>(selectedIndex));
	usedCard.push_back(selectedCard);
	return selectedCard;
}

template class CardDeck<SkillCard>;
template class CardDeck<AutoUseCard>;