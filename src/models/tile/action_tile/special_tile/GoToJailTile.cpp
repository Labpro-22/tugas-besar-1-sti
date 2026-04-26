#include "models/tile/action_tile/special_tile/GoToJailTile.hpp"
#include <iostream>
#include "views/GameViewInterface.hpp"

OnLandResult GoToJailTile::onLand(Player&, CommandInterface&, GameViewInterface& view) {
    view.showMessage("Kamu mendarat di petak Pergi Ke Penjara!\nPolisi menangkapmu! Bidakmu langsung dipindahkan ke Penjara.\n");
    return OnLandResult::TriggerMoveToJail;
}