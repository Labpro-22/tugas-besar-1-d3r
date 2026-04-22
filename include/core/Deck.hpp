#pragma once

#include <cstddef>
#include <utility>
#include <vector>

class SkillCard;
class AutoUseCard;

template <typename T>
class CardDeck {
private:
	std::vector<T*> availableCards;
	std::vector<T*> usedCard;

public:
	CardDeck() = default;
	CardDeck(CardDeck&&) noexcept = default;
	CardDeck& operator=(CardDeck&&) noexcept = default;
	virtual ~CardDeck() = default;

	void addCard(T* card);
	void shuffleDeck();
	void printDeck() const;
	T* topDeck();
	T* getRandomCard();
	T* removeAt(size_t index);
	size_t size() const;
	bool empty() const;
	const std::vector<T*>& getCards() const;
};

extern template class CardDeck<SkillCard>;
extern template class CardDeck<AutoUseCard>;
