#pragma once
#include "SpecialTile.hpp"
#include <string>

class FreeParkingTile : public SpecialTile {
    public:
        FreeParkingTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
            : SpecialTile(tileID, letterCode, tileName, colourBlock) {}
        
        void onLand(Player& p) override;
};
