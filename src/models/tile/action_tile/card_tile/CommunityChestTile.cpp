#include "models/tile/action_tile/card_tile/CommunityChestTile.hpp"
#include "models/player/Player.hpp"
#include "models/exception/GeneralException.hpp"
#include "views/GameViewInterface.hpp"
#include "views/CommandInterface.hpp"
#include "models/player/Player.hpp"
#include <iostream>
#include <string>
#include <limits>

OnLandResult CommunityChestTile::onLand(Player&, CommandInterface&, GameViewInterface& view) {
    view.showMessage("Kamu mendarat di Community Chest Tile!\n");
    return OnLandResult::TakeCommunityChest;
}