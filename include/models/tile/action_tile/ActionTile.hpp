#pragma once
#include "models/tile/Tile.hpp"
#include <string>

class ActionTile : public Tile {
    public:
        ActionTile(int tileID, std::string letterCode, std::string tileName)
            : Tile(tileID, letterCode, tileName, "DEFAULT") {}
        ~ActionTile() = default;
};
