#pragma once

#include "models/card/SkillCard.hpp"

class LassoCard : public SkillCard {
public:
	static const int kCardCount = 2;

	LassoCard();
	void activate(Player& owner) override;
};
