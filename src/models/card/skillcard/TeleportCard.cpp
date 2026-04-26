#include "models/card/skillcard/TeleportCard.hpp"

#include <iostream>
#include <limits>
#include "models/board/Board.hpp"
#include "models/player/Player.hpp"

TeleportCard::TeleportCard()
    : SkillCard("TeleportCard", "Berpindah ke petak manapun di papan permainan") {}

void TeleportCard::activate(Player& owner) {
    // const int boardSize = Board::getActiveBoardSize();
    // const int maxTileId = (boardSize > 0) ? (boardSize - 1) : 0;

    // int targetTile = 0;
    // while (true) {
    //     std::cout << "[TELEPORT] Masukkan ID petak tujuan (0-" << maxTileId << "): ";
    //     if (std::cin >> targetTile && targetTile >= 0 && targetTile <= maxTileId) {
    //         break;
    //     }

    //     std::cout << "Input tidak valid. Coba lagi.\n";
    //     std::cin.clear();
    //     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    // }

    // owner.setPosition(targetTile);
}
