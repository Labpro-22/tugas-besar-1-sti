#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/player/Player.hpp"

static std::map<std::string, int> g_ownerRailroadCount;

RailRoadTile::RailRoadTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : PropertyTile(tileID, letterCode, tileName, colourBlock, purchasePrice, mortgageValue)
{
    if (railRoadRentPrices_.empty())
    {
        // Default tunggu reader
        railRoadRentPrices_[1] = 25;
        railRoadRentPrices_[2] = 50;
        railRoadRentPrices_[3] = 100;
        railRoadRentPrices_[4] = 200;
    }
}

OnLandResult RailRoadTile::onLand(Player& p, CommandInterface&, GameViewInterface& view) {
    view.showMessage("Kamu mendarat di " + getTileName() + " (" + getLetterCode() + ")!\n");

    // === BELUM DIMILIKI ===
    if (getPropertyStatus() == BANK || getOwnerUsername() == "BANK") {
        setOwnerUsername(p.getUsername());
        setPropertyStatus(OWNED);
        p.addProperty(this);

        view.showMessage("Belum ada yang menginjaknya duluan, stasiun ini kini menjadi milikmu!\n");
        return OnLandResult::Done;
    }

    // === MORTGAGED ===
    if (getPropertyStatus() == MORTGAGED) {
        view.showMessage("Sedang digadaikan. Tidak ada sewa.\n");
        return OnLandResult::Done;
    }

    // === MILIK SENDIRI ===
    if (getOwnerUsername() == p.getUsername()) {
        view.showMessage("Ini milikmu sendiri.\n");
        return OnLandResult::Done;
    }
    
    return OnLandResult::TriggerTryToPayRent;
}

int RailRoadTile::calculateRentPrice(int countRailRoad) const {
    return railRoadRentPrices_.count(countRailRoad)
        ? railRoadRentPrices_[countRailRoad]
        : railRoadRentPrices_.rbegin()->second * festivalMultiplier_;
}
// getter n setter
void RailRoadTile::setRailRoadRentPrices(const std::map<int, int>& prices) {
    railRoadRentPrices_ = prices;
}

const std::map<int, int>& RailRoadTile::getRailRoadRentPrices()
{
    return railRoadRentPrices_;
}
