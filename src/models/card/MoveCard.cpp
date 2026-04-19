#include "models/card/MoveCard.hpp"

#include <random>
#include <string>

MoveCard::MoveCard()
    : SkillCard("MoveCard", ""),
      steps_(1) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::bernoulli_distribution sixSevenChance(0.07);
    if (sixSevenChance(gen))
    {
        steps_ = 67;
    }
    else
    {
        std::uniform_int_distribution<int> normalDist(1, 6);
        steps_ = normalDist(gen);
    }

    description = "Bergerak maju " + std::to_string(steps_) + " petak.";
}

void MoveCard::activate(Player& owner)
{
    owner.setPosition(owner.getPosition() + steps_);
}

int MoveCard::getSteps() const
{
    return steps_;
}
