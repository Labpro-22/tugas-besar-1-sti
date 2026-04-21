#include "models/card/skillcard/TeleportCard.hpp"

#include <iostream>
#include <limits>

TeleportCard::TeleportCard()
    : SkillCard("TeleportCard", "Berpindah ke petak manapun di papan permainan") {}

void TeleportCard::activate(Player& owner) {
    int targetTile = 0;
    while (true) {
        std::cout << "[TELEPORT] Masukkan ID petak tujuan (>= 0): ";
        if (std::cin >> targetTile && targetTile >= 0) {
            break;
        }

        std::cout << "Input tidak valid. Coba lagi.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    owner.setPosition(targetTile);
}
