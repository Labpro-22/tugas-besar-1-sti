#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <memory>
#include <random>
#include <utility>

template <typename T>
class Deck {
	private:
		std::stack<std::unique_ptr<T>> drawPile_;
		std::vector<std::unique_ptr<T>> discardPile_;
		int capacity_;

	public:
		Deck(int capacity = 0);

		// Inisialisasi deck dengan kumpulan kartu berpemilik
		void initDeck(std::vector<std::unique_ptr<T>> initialCards);

		// Mengambil kartu (Otomatis reshuffle jika drawPile kosong)
		std::unique_ptr<T> drawDeck();

		// Memasukkan kartu kembali ke tumpukan buangan
		void pushToDiscard(std::unique_ptr<T> item);

		// Mengacak kartu menggunakan Mersenne Twister
		void shuffleDeck();

		int getDrawCount() const;
		int getDiscardCount() const;
};

