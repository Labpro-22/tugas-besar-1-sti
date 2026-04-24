#pragma once

#include "models/card/skillcard/SkillCard.hpp"

class Player;

class ShieldCard : public SkillCard {
	private:
		int remainingTurns_;

	public:
		static const int kCardCount = 2;

		ShieldCard();
		void activate(Player& owner) override;

		bool isActive() const;
		int getRemainingTurns() const;
		void consumeTurn();
};
