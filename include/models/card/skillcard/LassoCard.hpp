#pragma once

#include "models/card/skillcard/SkillCard.hpp"

#include <iostream>
#include <limits>

class LassoCard : public SkillCard {
	public:
		static const int kCardCount = 2;

		LassoCard();
		void activate(Player& owner) override;
};
