#pragma once
#include "SpecialTile.hpp"
#include <string>

class PrisonTile : public SpecialTile {
    private:
        int fineCost_;
    public:
        PrisonTile(int tileID, std::string letterCode, std::string tileName, int fineCost)
            : SpecialTile(tileID, letterCode, tileName), fineCost_(fineCost) {}
        ~PrisonTile() = default;
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
        int getFineCost() const { return fineCost_; }
};