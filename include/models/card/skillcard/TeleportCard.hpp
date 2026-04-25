#pragma once

#include "models/card/skillcard/SkillCard.hpp"
#include <iostream>
#include <limits>

class TeleportCard : public SkillCard {
	public:
		static const int kCardCount = 2;

		TeleportCard();
		void activate(Player& owner) override;
};
