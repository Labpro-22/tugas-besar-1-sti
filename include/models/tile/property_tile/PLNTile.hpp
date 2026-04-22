#pragma once
#include "UtilityTile.hpp"

class PLNTile : public UtilityTile{
    public:
        PLNTile(int tileID, std::string letterCode,
                std::string tileName, std::string colourBlock, 
                int purchasePrice, int mortgageValue);
        ~PLNTile();
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
};