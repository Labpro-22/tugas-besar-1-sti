#include "models/card/LassoCard.hpp"

#include <iostream>

LassoCard::LassoCard()
    : SkillCard("Lasso Card", "Menarik satu lawan di depan ke petak pemain saat ini") {}

void LassoCard::activate(Player& owner)
{
    (void)owner;
    std::cout << "[LASSO] tungguin player bg.\n";
}
