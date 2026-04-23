#pragma once
#include "CardTile.hpp"
#include <string>

class ChanceTile : public CardTile {
    public:
        ChanceTile(int tileID, std::string letterCode, std::string tileName)
            : CardTile(tileID, letterCode, tileName) {}
        ~ChanceTile() = default;
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
};