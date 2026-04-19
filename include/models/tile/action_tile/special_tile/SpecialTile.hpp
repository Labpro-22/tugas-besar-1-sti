#pragma once
#include "models/tile/action_tile/ActionTile.hpp"
#include <string>

// ==========================================
class SpecialTile : public ActionTile {
    public:
        SpecialTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
            : ActionTile(tileID, letterCode, tileName, colourBlock) {}
        
        virtual void executeTile(Player* p) override = 0;
};
