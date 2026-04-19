#pragma once

#include "models/card/SkillCard.hpp"

class DiscountCard : public SkillCard {
	private:
		int discountPercent_;
		int remainingTurns_;

	public:
		static const int kCardCount = 3;

		DiscountCard();
		void activate(Player& owner) override;

		int getDiscountPercent() const;
		int getRemainingTurns() const;
		void consumeTurn();
};
