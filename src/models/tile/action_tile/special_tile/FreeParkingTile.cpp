#include "models/tile/action_tile/special_tile/FreeParkingTile.hpp"
#include "models/player/Player.hpp"
#include <iostream>

OnLandResult FreeParkingTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    std::cout << "Kamu mendarat di petak Bebas Parkir!\n";
    std::cout << "Beristirahatlah sejenak...\n";
    std::cout << "---\n";
    return OnLandResult::Done;
}