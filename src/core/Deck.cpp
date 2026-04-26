#include "../../include/core/Deck.hpp"

#include <algorithm>
#include <random>
#include <cstddef>

#include "../../include/core/Card.hpp"
#include "../../include/core/GameManager.hpp"

template <typename T>
void CardDeck<T>::addCard(T* card) {
	if (card != nullptr) {
		availableCards.push_back(card);
	}
}

template <typename T>
void CardDeck<T>::addUsedCard(T* card) {
	if (card != nullptr) {
		usedCard.push_back(card);
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
			GameManager::getInstance().writeLine(card->getCardName());
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

template <typename T>
T* CardDeck<T>::removeAt(size_t index) {
	if (index >= availableCards.size()) {
		return nullptr;
	}

	T* selectedCard = availableCards[index];
	availableCards.erase(availableCards.begin() + static_cast<long long>(index));
	return selectedCard;
}

template <typename T>
size_t CardDeck<T>::size() const {
	return availableCards.size();
}

template <typename T>
bool CardDeck<T>::empty() const {
	return availableCards.empty();
}

template <typename T>
const std::vector<T*>& CardDeck<T>::getCards() const {
	return availableCards;
}

template class CardDeck<SkillCard>;
template class CardDeck<AutoUseCard>;
