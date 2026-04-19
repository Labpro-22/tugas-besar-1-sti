#pragma once
#include "CardTile.hpp"
#include <string>

class ChanceTile : public CardTile {
    public:
        ChanceTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
            : CardTile(tileID, letterCode, tileName, colourBlock) {}
            
        void executeTile(Player* p) override;
};