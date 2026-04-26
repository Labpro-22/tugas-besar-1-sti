#include "models/tile/action_tile/special_tile/FreeParkingTile.hpp"
#include "views/GameViewInterface.hpp"
#include <iostream>

OnLandResult FreeParkingTile::onLand(Player&, CommandInterface&, GameViewInterface& view) {
    view.showMessage("Kamu di free parking tile!\n");
    return OnLandResult::Done;
}