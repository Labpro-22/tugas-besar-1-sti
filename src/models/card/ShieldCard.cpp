#include "models/card/ShieldCard.hpp"

ShieldCard::ShieldCard()
    : SkillCard("ShieldCard", "Melindungi pemain dari sanksi merugikan selama 1 giliran"),
      remainingTurns_(1) {}

void ShieldCard::activate(Player& owner)
{
    // TODO: Integrasi Player
    (void)owner;
}

bool ShieldCard::isActive() const
{
    return remainingTurns_ > 0;
}

int ShieldCard::getRemainingTurns() const
{
    return remainingTurns_;
}

void ShieldCard::consumeTurn()
{
    if (remainingTurns_ > 0)
    {
        --remainingTurns_;
    }
}
