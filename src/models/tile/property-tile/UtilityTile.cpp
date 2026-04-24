#include "models/tile/property_tile/UtilityTile.hpp"
#include "models/player/Player.hpp"

static std::map<std::string, int> g_ownerUtilityCount;

UtilityTile::UtilityTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : PropertyTile(tileID, letterCode, tileName, colourBlock, purchasePrice, mortgageValue)
{
    if (utilityFactor_.empty())
    {
        // Default tunggu reader
        utilityFactor_[1] = 4;
        utilityFactor_[2] = 10;
    }
}

OnLandResult UtilityTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    (void) command;

    view.showMessage("Kamu mendarat di " + getTileName() + "\n");

    // === BELUM DIMILIKI ===
    if (getPropertyStatus() == BANK || getOwnerUsername() == "BANK") {
        setOwnerUsername(p.getUsername());
        setPropertyStatus(OWNED);
        p.addProperty(this);

        view.showMessage(getTileName() + " kini menjadi milikmu!\n");
        return OnLandResult::Done;
    }

    if (getPropertyStatus() == MORTGAGED) {
        view.showMessage("Sedang digadaikan.\n");
        return OnLandResult::Done;
    }

    if (getOwnerUsername() == p.getUsername()) {
        view.showMessage("Milik sendiri.\n");
        return OnLandResult::Done;
    }

    return OnLandResult::TriggerTryToPayRent;
}

int UtilityTile::calculateRentPrice(int countUtils, int diceResult) {
    int factor = utilityFactor_.count(countUtils)
        ? utilityFactor_[countUtils]
        : utilityFactor_.rbegin()->second;
    return diceResult * factor * festivalMultiplier_;
}

// getter n setter
void UtilityTile::setUtilityFactor(const std::map<int, int>& factors)
{
    utilityFactor_ = factors;
}

const std::map<int, int>& UtilityTile::getUtilityFactor()
{
    return utilityFactor_;
}
