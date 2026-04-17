#pragma once

#include "models/card/SkillCard.hpp"

class MoveCard : public SkillCard {
private:
	int steps_;

public:
	static constexpr int kCardCount = 4;

	MoveCard();
	void activate(Player& owner) override;

	int getSteps() const;
};
