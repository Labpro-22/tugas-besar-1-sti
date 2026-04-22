#pragma once
#include "SpecialTile.hpp"
#include <string>

class FreeParkingTile : public SpecialTile {
    public:
        FreeParkingTile(int tileID, std::string letterCode, std::string tileName)
            : SpecialTile(tileID, letterCode, tileName) {}
        
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
};
