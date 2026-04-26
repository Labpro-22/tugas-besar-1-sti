#include "models/tile/action_tile/special_tile/PrisonTile.hpp"
#include "models/player/Player.hpp"
#include "core/Monopoly.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

OnLandResult PrisonTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    view.showMessage("Kamu mendarat di Penjara. Kamu ditangkap oleh polisi!\n");
    return OnLandResult::TriggerMoveToJail;
}
