#pragma once
#include "ActionTile.hpp"
#include <string>

class FestivalTile : public ActionTile {
    public:
        FestivalTile(int tileID, std::string letterCode, std::string tileName)
            : ActionTile(tileID, letterCode, tileName) {}
        ~FestivalTile() = default;
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
};