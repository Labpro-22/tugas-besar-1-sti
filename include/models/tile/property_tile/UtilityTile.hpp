#pragma once
#include <algorithm>
#include <map>
#include <sstream>
#include "RailRoadTile.hpp"
#include "models/player/Player.hpp"
#include "views/GameViewInterface.hpp"
#include "views/CommandInterface.hpp"

class UtilityTile : public PropertyTile {
    private:
        static inline std::map<int, int> utilityFactor_;
    public:
        UtilityTile(int tileID, std::string letterCode,
                std::string tileName, std::string colourBlock, 
                int purchasePrice, int mortgageValue);
        virtual ~UtilityTile() = default;

        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;

        int calculateRentPrice(int countUtils, int diceResult);

        // getter n setter
        static void setUtilityFactor(const std::map<int, int>& factors);
        static const std::map<int, int>& getUtilityFactor();
};
