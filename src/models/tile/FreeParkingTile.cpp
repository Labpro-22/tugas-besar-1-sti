#include "models/tile/FreeParkingTile.hpp"
#include "models/player/Player.hpp"
#include <iostream>

void FreeParkingTile::executeTile(Player* p) {
    std::cout << "Kamu mendarat di petak Bebas Parkir!\n";
    std::cout << "Beristirahatlah sejenak...\n";
    std::cout << "---\n";
}