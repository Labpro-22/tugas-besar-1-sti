#pragma once

#include "models/card/skillcard/SkillCard.hpp"

class MoveCard : public SkillCard {
	private:
		int steps_;

	public:
		static const int kCardCount = 4;

		MoveCard();
		void activate(Player& owner) override;

		int getSteps() const;
};
