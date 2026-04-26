#pragma once
#include "models/tile/action_tile/ActionTile.hpp"
#include <string>

class TaxTile : public ActionTile {
    public:
        TaxTile(int tileID, std::string letterCode, std::string tileName)
            : ActionTile(tileID, letterCode, tileName) {}
        ~TaxTile() = default;
};