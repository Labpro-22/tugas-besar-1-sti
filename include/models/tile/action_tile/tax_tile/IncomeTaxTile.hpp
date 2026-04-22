#pragma once
#include "TaxTile.hpp"
#include <string>

class IncomeTaxTile : public TaxTile {
    private:
        static inline int flatCost_ = 0;
        static inline float taxPercentage_ = 0;
    public:
        IncomeTaxTile(int tileID, std::string letterCode, std::string tileName)
            : TaxTile(tileID, letterCode, tileName) {}
        
        static void setPPHFlatCost(int cost);
        static void setTaxPercentage(float percentage);
        int getPPHFlatCost() const;
        float getTaxPercentage() const;
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
};