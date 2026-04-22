#pragma once
#include <map>
#include "PropertyTile.hpp"
class StreetTile : public PropertyTile {
    private:
        // color group (khusus Street) perlu?? atau cukup colorCode
        std::map<int, int> buildPrice_; //  serta harga bangunan per rumah dan hotel
        std::map<int, int> rentPricePerLevel_; // Key : Level, Value : Rent Price

    public:
        StreetTile(int tileID, std::string letterCode,
                std::string tileName, std::string colourBlock, 
                int purchasePrice, int mortgageValue,
                // khusus street
                std::map<int, int> rentPricePerLevel, // Key : Level, Value : Rent Price
                std::map<int, int> buildPrice_);
        ~StreetTile();
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;

        std::map<int, int> getBuildPrice() const;
};
