#pragma once

#include "models/card/skillcard/SkillCard.hpp"
#include <iostream>
#include <random>

class DiscountCard : public SkillCard {
	private:
		int discountPercent_;
		int remainingTurns_;

	public:
		static const int kCardCount = 3;

		DiscountCard();
		DiscountCard(int discountPercent, int remainingTurns);
		void activate(Player& owner) override;

		int getDiscountPercent() const;
		int getRemainingTurns() const;
		void consumeTurn();
};
