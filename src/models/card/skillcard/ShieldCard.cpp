#include "models/card/skillcard/ShieldCard.hpp"

#include <iostream>

ShieldCard::ShieldCard()
    : SkillCard("ShieldCard", "Melindungi pemain dari sanksi merugikan selama 1 giliran"),
      remainingTurns_(1) {}

void ShieldCard::activate(Player& owner) {
    (void)owner;
    remainingTurns_ = 1;
    std::cout << "[SHIELD] Perlindungan aktif selama 1 giliran.\n";
}

bool ShieldCard::isActive() const {
    return remainingTurns_ > 0;
}

int ShieldCard::getRemainingTurns() const {
    return remainingTurns_;
}

void ShieldCard::consumeTurn() {
    if (remainingTurns_ > 0) {
        --remainingTurns_;
    }
}
