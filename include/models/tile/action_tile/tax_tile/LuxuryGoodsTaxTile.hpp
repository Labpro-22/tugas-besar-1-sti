#pragma once
#include "TaxTile.hpp"
#include "models/player/Player.hpp"
#include "models/exception/GeneralException.hpp"
#include <iostream>
#include <string>
#include <limits>

class LuxuryGoodsTaxTile : public TaxTile {
    private:
        static inline int flatCost_ = 0;
    public:
        LuxuryGoodsTaxTile(int tileID, std::string letterCode, std::string tileName)
            : TaxTile(tileID, letterCode, tileName) {}
            ~LuxuryGoodsTaxTile() = default;
        static void setPBMFlatCost(int cost);
        int getPBMFlatCost() const;
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
};