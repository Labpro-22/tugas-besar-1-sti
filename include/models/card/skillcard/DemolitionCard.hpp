#pragma once

#include "models/card/skillcard/SkillCard.hpp"
#include <iostream>
#include <limits>
#include <vector>

class DemolitionCard : public SkillCard {
	public:
		static const int kCardCount = 2;

		DemolitionCard();
		void activate(Player& owner) override;
};
