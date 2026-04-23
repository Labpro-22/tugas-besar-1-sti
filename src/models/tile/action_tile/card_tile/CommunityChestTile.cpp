#include "models/tile/action_tile/card_tile/CommunityChestTile.hpp"
#include "models/player/Player.hpp"
#include "models/exception/Exception.hpp"
#include <iostream>
#include <string>
#include <limits>

OnLandResult CommunityChestTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    view.showMessage("Kamu mendarat di Community Chest Tile!\n");
    return OnLandResult::TakeCommunityChest;
}