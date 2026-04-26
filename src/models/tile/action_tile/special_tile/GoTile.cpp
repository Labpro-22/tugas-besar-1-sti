#include "models/tile/action_tile/special_tile/GoTile.hpp"
#include "models/player/Player.hpp"
#include <iostream>

OnLandResult GoTile::onLand(Player& p, CommandInterface&, GameViewInterface& view) {
    view.showMessage("Kamu mendarat di GoTile, kamu mendapatkan uang sebanyak X!\n");
    
    p.addMoney(getSalary());

    view.showMessage("Uang kamu sebesar X\n");
    return OnLandResult::Done;
}