#pragma once
#include <map>
#include "RailRoadTile.hpp"
class UtilityTile : public PropertyTile {
    private:
        static inline std::map<int, int> utilityFactor_;
    public:
        UtilityTile(int tileID, std::string letterCode,
                std::string tileName, std::string colourBlock, 
                int purchasePrice, int mortgageValue);
        virtual ~UtilityTile();
};
