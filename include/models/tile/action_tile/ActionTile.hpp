#pragma once
#include "models/tile/Tile.hpp"
#include <string>

class ActionTile : public Tile {
    public:
        ActionTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
            : Tile(tileID, letterCode, tileName, colourBlock) {}
};
