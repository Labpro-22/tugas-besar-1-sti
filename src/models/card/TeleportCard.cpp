#include "models/card/TeleportCard.hpp"

TeleportCard::TeleportCard()
    : SkillCard("Teleport Card", "Berpindah ke petak manapun di papan permainan") {}

void TeleportCard::activate(Player& owner) {
    // TODO: Integrasi Player
    (void)owner;
}
