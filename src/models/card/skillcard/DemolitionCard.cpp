#include "models/card/skillcard/DemolitionCard.hpp"

#include <iostream>

DemolitionCard::DemolitionCard()
    : SkillCard("Demolition Card", "Menghancurkan satu properti milik pemain lawan") {}

void DemolitionCard::activate(Player& owner) {
    (void)owner;
    std::cout << "[DEMOLITION] tungguin player.\n";
}
