#include "../../../include/models/card/Deck.hpp"

template <typename T>
Deck<T>::Deck(int capacity) : capacity_(capacity) {}

template <typename T>
void Deck<T>::initDeck(std::vector<std::unique_ptr<T>> initialCards) {
	while (!drawPile_.empty()) {
		drawPile_.pop();
	}
	discardPile_.clear();

	for (auto& card : initialCards) {
		discardPile_.push_back(std::move(card));
	}
	capacity_ = static_cast<int>(discardPile_.size());
	shuffleDeck();
}

template <typename T>
std::unique_ptr<T> Deck<T>::drawDeck() {
	if (drawPile_.empty()) {
		if (discardPile_.empty()) return {};
		shuffleDeck();
	}

	std::unique_ptr<T> card = std::move(drawPile_.top());
	drawPile_.pop();
	return card;
}

template <typename T>
void Deck<T>::pushToDiscard(std::unique_ptr<T> item) {
	if (item) {
		discardPile_.push_back(std::move(item));
	}
}

template <typename T>
void Deck<T>::shuffleDeck() {
	if (discardPile_.empty()) {
		return;
	}

	static std::random_device rd;
	static std::mt19937 g(rd());

	std::shuffle(discardPile_.begin(), discardPile_.end(), g);

	for (auto& card : discardPile_)
    {
		drawPile_.push(std::move(card));
	}
	discardPile_.clear();
}

template <typename T>
int Deck<T>::getDrawCount() const {
	return static_cast<int>(drawPile_.size());
}

template <typename T>
int Deck<T>::getDiscardCount() const {
	return static_cast<int>(discardPile_.size());
}

#include "models/card/skillcard/SkillCard.hpp"
template class Deck<SkillCard>;