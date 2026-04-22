#pragma once
#include "SpecialTile.hpp"
#include <string>

class GoToJailTile : public SpecialTile {
    public:
        GoToJailTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
            : SpecialTile(tileID, letterCode, tileName, colourBlock) {}
        
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
};