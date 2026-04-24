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

UtilityTile::~UtilityTile() {}

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

    Player* owner = nullptr;
    for (Player* pl : p.getAllPlayers()) {
        if (pl != nullptr && pl->getUsername() == getOwnerUsername()) {
            owner = pl;
            break;
        }
    }

    int count = 1;
    if (owner != nullptr) {
        count = owner->countUtilities();
        if (count < 1) count = 1;
    }

    int factor = utilityFactor_.count(count)
        ? utilityFactor_[count]
        : utilityFactor_.rbegin()->second;

    int dice = p.getLastDiceTotal();
    int rent = dice * factor;

    view.showMessage("Total dadu: " + std::to_string(dice) + "\n");
    view.showMessage("Faktor: " + std::to_string(factor) + "\n");
    view.showMessage("Sewa: M" + std::to_string(rent) + "\n");

    if (p.getBalance() < rent) {
        return OnLandResult::TriggerBankruptcyAuction;
    }

    p.deductMoney(rent);
    if (owner) owner->addMoney(rent);

    return OnLandResult::Done;
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
