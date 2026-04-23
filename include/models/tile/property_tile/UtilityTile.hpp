#pragma once
#include <algorithm>
#include <map>
#include <sstream>
#include "RailRoadTile.hpp"
class UtilityTile : public PropertyTile {
    private:
        static inline std::map<int, int> utilityFactor_;
    public:
        UtilityTile(int tileID, std::string letterCode,
                std::string tileName, std::string colourBlock, 
                int purchasePrice, int mortgageValue);
        virtual ~UtilityTile();

        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;

        // getter n setter
        static void setUtilityFactor(const std::map<int, int>& factors);
        static const std::map<int, int>& getUtilityFactor();
};
