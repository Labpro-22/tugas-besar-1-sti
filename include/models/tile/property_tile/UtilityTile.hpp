#pragma once
#include <algorithm>
#include <map>
#include <sstream>
#include "RailRoadTile.hpp"
class UtilityTile : public PropertyTile {
    private:
        static inline std::map<int, int> utilityFactor_;
        static inline std::map<std::string, int> ownerUtilityCount_;
    public:
        UtilityTile(int tileID, std::string letterCode,
                std::string tileName, std::string colourBlock, 
                int purchasePrice, int mortgageValue);
        virtual ~UtilityTile();

        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;

        // getter n setter
        static void setUtilityFactor(const std::map<int, int>& factors);
        static const std::map<int, int>& getUtilityFactor();
        
        // untuk menghitung jumlah utility owner
        static void incrementOwnerCount(const std::string& username);
        static void decrementOwnerCount(const std::string& username);
        static int getOwnerCount(const std::string& username);
};
