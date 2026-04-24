#include "models/tile/action_tile/card_tile/ChanceTile.hpp"
#include "models/player/Player.hpp"
#include "models/exception/Exception.hpp"
#include <iostream>
#include <string>
#include <limits>

OnLandResult ChanceTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    view.showMessage("Kamu mendarat di Petak Kesempatan!\n");
    return OnLandResult::TakeChanceCard;
}