#pragma once
#include <algorithm>
#include <map>
#include <sstream>
#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/player/Player.hpp"
#include "views/GameViewInterface.hpp"
#include "views/CommandInterface.hpp"

class RailRoadTile : public PropertyTile {
    private:
        static inline std::map<int, int> railRoadRentPrices_;
    public:
        // perlu method static untuk set value map-nya waktu ngeload config?? :/
        RailRoadTile(int tileID, std::string letterCode,
                    std::string tileName, std::string colourBlock,
                    int purchasePrice, int mortgageValue);
        ~RailRoadTile() = default;
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;

        int calculateRentPrice(int countRailRoad) const;

        // getter n setter
        static void setRailRoadRentPrices(const std::map<int, int>& prices);
        static const std::map<int, int>& getRailRoadRentPrices();
};
