#pragma once
#include "SpecialTile.hpp"
#include <string>

class PrisonTile : public SpecialTile {
    private:
        int fineCost_;
    public:
        PrisonTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int fineCost)
            : SpecialTile(tileID, letterCode, tileName, colourBlock), fineCost_(fineCost) {}
        
        void executeTile(Player* p) override;
        int tryExitPrison(Player *p);
        int getFineCost() const { return fineCost_; }
};