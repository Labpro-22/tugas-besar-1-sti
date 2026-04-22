#pragma once
#include "UtilityTile.hpp"

class PAMTile : public UtilityTile{
    public:
        PAMTile(int tileID, std::string letterCode,
                std::string tileName, std::string colourBlock, 
                int purchasePrice, int mortgageValue);
        ~PAMTile();
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
};