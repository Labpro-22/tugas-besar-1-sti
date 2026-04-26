#include "models/tile/action_tile/special_tile/PrisonTile.hpp"
#include "models/player/Player.hpp"
#include "core/Monopoly.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

OnLandResult PrisonTile::onLand(Player&, CommandInterface&, GameViewInterface& view) {
    view.showMessage("Kamu mendarat di Penjara!\nUntungnya kamu hanya lewat dari sana.\n");
    return OnLandResult::Done;
}
