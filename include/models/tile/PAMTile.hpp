#pragma once
#include "UtilityTile.hpp"

class PAMTile {
    public:
        PAMTile(int tileID, std::string letterCode,
                std::string tileName, std::string colourBlock, 
                int purchasePrice, int mortgageValue);
        ~PAMTile();
};