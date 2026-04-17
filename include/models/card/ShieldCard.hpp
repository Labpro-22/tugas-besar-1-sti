#pragma once

#include "models/card/SkillCard.hpp"

class ShieldCard : public SkillCard {
private:
	int remainingTurns_;

public:
	static constexpr int kCardCount = 2;

	ShieldCard();
	void activate(Player& owner) override;

	bool isActive() const;
	int getRemainingTurns() const;
	void consumeTurn();
};
