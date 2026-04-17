#include "models/card/DemolitionCard.hpp"

DemolitionCard::DemolitionCard()
    : SkillCard("Demolition Card", "Menghancurkan satu properti milik pemain lawan") {}

void DemolitionCard::activate(Player& owner) {
    // TODO: Integrasi Player + Monopoly
    (void)owner;
}
