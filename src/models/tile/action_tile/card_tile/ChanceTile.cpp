#include "models/tile/action_tile/card_tile/ChanceTile.hpp"
#include "models/player/Player.hpp"
#include "models/exception/GeneralException.hpp"
#include "views/GameViewInterface.hpp"
#include "views/CommandInterface.hpp"
#include "models/player/Player.hpp"
#include <iostream>
#include <string>
#include <limits>

OnLandResult ChanceTile::onLand(Player&, CommandInterface&, GameViewInterface& view) {
    view.showMessage("Kamu mendarat di Petak Kesempatan!\n");
    return OnLandResult::TakeChanceCard;
}