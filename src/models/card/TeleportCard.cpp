#include "models/card/TeleportCard.hpp"

TeleportCard::TeleportCard()
    : SkillCard("TeleportCard", "Berpindah ke petak manapun di papan permainan") {}

void TeleportCard::activate(Player& owner)
{
    owner.allowTeleport();
}
