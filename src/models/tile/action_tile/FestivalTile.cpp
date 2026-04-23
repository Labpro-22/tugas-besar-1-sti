#include "models/tile/action_tile/FestivalTile.hpp"
#include "models/player/Player.hpp"
#include "models/exception/Exception.hpp"
#include <iostream>
#include <string>
#include <limits>

OnLandResult FestivalTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    view.showMessage("Hi!\n Kamu ada di petak Festival!\n");
    if (p.getProperties().empty()) {
        // artinya langsung balik kelar
        view.showMessage("Yah!\n Sayang sekali kamu tidak punya apa apa, tapi tenang kamu punya kami kok ahihihihihihi");
        return OnLandResult::Done;
    }
    return OnLandResult::Festival;
}