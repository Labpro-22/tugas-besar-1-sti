#include "models/tile/action_tile/special_tile/PrisonTile.hpp"
#include "models/player/Player.hpp"
#include "core/Monopoly.hpp"
#include <iostream>
#include <limits>

OnLandResult PrisonTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    view.showMessage("Yahahaha ga ngapain\" lanjut jalan sana gih\n");
    return OnLandResult::Done;
}
