#include "../../../include/models/card/Deck.hpp"


template <typename T>
Deck<T>::Deck(int capacity) : capacity_(capacity) {}

template <typename T>
void Deck<T>::initDeck(const std::vector<T*>& initialCards)
{
	while (!drawPile_.empty()) drawPile_.pop();
	discardPile_.clear();

	for (T* card : initialCards)
    {
		discardPile_.push_back(card);
	}
	capacity_ = static_cast<int>(discardPile_.size());
	shuffleDeck();
}

template <typename T>
T* Deck<T>::drawDeck()
{
	if (drawPile_.empty())
    {
		if (discardPile_.empty()) return nullptr;
		shuffleDeck();
	}

	T* card = drawPile_.top();
	drawPile_.pop();
	return card;
}

template <typename T>
void Deck<T>::pushToDiscard(T* item)
{
	if (item)
    {
		discardPile_.push_back(item);
	}
}

template <typename T>
void Deck<T>::shuffleDeck()
{
	if (discardPile_.empty()) return;

	static std::random_device rd;
	static std::mt19937 g(rd());

	std::shuffle(discardPile_.begin(), discardPile_.end(), g);

	for (T* card : discardPile_)
    {
		drawPile_.push(card);
	}
	discardPile_.clear();
}

template <typename T>
int Deck<T>::getDrawCount() const
{
	return static_cast<int>(drawPile_.size());
}

template <typename T>
int Deck<T>::getDiscardCount() const
{
	return static_cast<int>(discardPile_.size());
}