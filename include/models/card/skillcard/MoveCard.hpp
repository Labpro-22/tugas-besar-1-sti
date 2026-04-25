#pragma once

#include "models/card/skillcard/SkillCard.hpp"
#include <random>
#include <string>

class Player;

class MoveCard : public SkillCard {
	private:
		int steps_;

	public:
		static const int kCardCount = 4;

		MoveCard();
		MoveCard(int steps);
		void activate(Player& owner) override;

		int getSteps() const;
};
