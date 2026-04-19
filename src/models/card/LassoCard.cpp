#include "models/card/LassoCard.hpp"

LassoCard::LassoCard()
    : SkillCard("Lasso Card", "Menarik satu lawan di depan ke petak pemain saat ini") {}

void LassoCard::activate(Player& owner)
{
    owner.useLasso();
}
