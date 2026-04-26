#include "models/card/skillcard/TeleportCard.hpp"

#include <iostream>
#include <limits>
#include "models/board/Board.hpp"
#include "models/player/Player.hpp"

TeleportCard::TeleportCard()
    : SkillCard("TeleportCard", "Berpindah ke petak manapun di papan permainan") {}

void TeleportCard::activate(Player&) {
}
