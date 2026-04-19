#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <random>

template <typename T>
class Deck {
private:
	std::stack<T*> drawPile_;
	std::vector<T*> discardPile_;
	int capacity_;

public:
	Deck(int capacity = 0);

	// Inisialisasi deck dengan kumpulan pointer kartu
	void initDeck(const std::vector<T*>& initialCards);

	// Mengambil kartu (Otomatis reshuffle jika drawPile kosong)
	T* drawDeck();

	// Memasukkan kartu kembali ke tumpukan buangan
	void pushToDiscard(T* item);

	// Mengacak kartu menggunakan Mersenne Twister
	void shuffleDeck();

	int getDrawCount() const;
	int getDiscardCount() const;
};

#include "../../../src/models/card/Deck.cpp"

