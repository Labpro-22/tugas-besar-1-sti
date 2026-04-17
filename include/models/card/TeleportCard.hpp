#pragma once

#include "models/card/SkillCard.hpp"

class TeleportCard : public SkillCard {
public:
	static constexpr int kCardCount = 2;

	TeleportCard();
	void activate(Player& owner) override;
};
