#include "models/card/skillcard/MoveCard.hpp"
#include "models/player/Player.hpp"

MoveCard::MoveCard()
    : SkillCard("MoveCard", ""),
      steps_(1) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::bernoulli_distribution sixSevenChance(0.07);
    if (sixSevenChance(gen)) {
        steps_ = 67;
    }
    else {
        std::uniform_int_distribution<int> normalDist(1, 12);
        steps_ = normalDist(gen);
    }

    description = "Bergerak maju " + std::to_string(steps_) + " petak.";
}

MoveCard::MoveCard(int steps)
    : SkillCard("MoveCard", "")
    , steps_(steps) {
    description = "Bergerak maju " + std::to_string(steps_) + " petak.";
}

void MoveCard::activate(Player& owner) {
    owner.setPosition(owner.move(steps_));
}

int MoveCard::getSteps() const {
    return steps_;
}
