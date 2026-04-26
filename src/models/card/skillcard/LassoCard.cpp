#include "models/card/skillcard/LassoCard.hpp"
#include "models/player/Player.hpp"

LassoCard::LassoCard()
    : SkillCard("Lasso Card", "Menarik satu lawan di depan ke petak pemain saat ini") {}

void LassoCard::activate(Player&) {
}
