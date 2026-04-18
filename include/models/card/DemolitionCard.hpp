#pragma once

#include "models/card/SkillCard.hpp"

class DemolitionCard : public SkillCard {
public:
	static const int kCardCount = 2;

	DemolitionCard();
	void activate(Player& owner) override;
};
