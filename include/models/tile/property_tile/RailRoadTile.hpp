#pragma once
#include <algorithm>
#include <map>
#include <sstream>
#include "models/tile/property_tile/PropertyTile.hpp"
class RailRoadTile : public PropertyTile {
    private:
        static inline std::map<int, int> railRoadRentPrices_;
        static inline std::map<std::string, int> ownerRailroadCount_; // simpan jumlah railroad yang dimiliki player yang jatuh di railroad ini
    public:
        // perlu method static untuk set value map-nya waktu ngeload config?? :/
        RailRoadTile(int tileID, std::string letterCode,
                    std::string tileName, std::string colourBlock,
                    int purchasePrice, int mortgageValue);
        ~RailRoadTile();
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;

        // getter n setter
        static void setRailRoadRentPrices(const std::map<int, int>& prices);
        static const std::map<int, int>& getRailRoadRentPrices();

        // keperluan jumlah railroad yang dimiliki owner
        static void incrementOwnerCount(const std::string& username);
        static void decrementOwnerCount(const std::string& username);
        static int getOwnerCount(const std::string& username);
};
