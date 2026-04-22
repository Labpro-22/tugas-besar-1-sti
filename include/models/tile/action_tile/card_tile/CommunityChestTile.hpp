#pragma once
#include "CardTile.hpp"
#include <string>

class CommunityChestTile : public CardTile {
    public:
        CommunityChestTile(int tileID, std::string letterCode, std::string tileName)
            : CardTile(tileID, letterCode, tileName) {}
            
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
};