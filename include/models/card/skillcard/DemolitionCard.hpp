#pragma once

#include "models/card/skillcard/SkillCard.hpp"

class Player;

class DemolitionCard : public SkillCard {
	public:
		static const int kCardCount = 2;

		DemolitionCard();
		void activate(Player& owner) override;
};
