#pragma once
#include "CardTile.hpp"
#include <string>

class ChanceTile : public CardTile {
    public:
        ChanceTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
            : CardTile(tileID, letterCode, tileName, colourBlock) {}
            
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
};