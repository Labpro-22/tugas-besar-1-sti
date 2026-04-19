#pragma once
#include "ActionTile.hpp"
#include <string>

class FestivalTile : public ActionTile {
    public:
        FestivalTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
            : ActionTile(tileID, letterCode, tileName, colourBlock) {}
            
        void executeTile(Player* p) override;
};