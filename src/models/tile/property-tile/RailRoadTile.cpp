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

RailRoadTile::~RailRoadTile() {}

OnLandResult RailRoadTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    (void) command;

    view.showMessage("Kamu mendarat di " + getTileName() + " (" + getLetterCode() + ")!\n");

    // === BELUM DIMILIKI ===
    if (getPropertyStatus() == BANK || getOwnerUsername() == "BANK") {
        setOwnerUsername(p.getUsername());
        setPropertyStatus(OWNED);
        p.addProperty(this);

        view.showMessage("Stasiun ini kini menjadi milikmu!\n");
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

    // === AMBIL OWNER ===
    Player* owner = nullptr;
    for (Player* pl : Player::getAllPlayers()) {
        if (pl != nullptr && pl->getUsername() == getOwnerUsername()) {
            owner = pl;
            break;
        }
    }

    int count = 1;
    if (owner != nullptr) {
        count = owner->countRailroad();
        if (count < 1) count = 1;
    }

    int rent = railRoadRentPrices_.count(count)
        ? railRoadRentPrices_[count]
        : railRoadRentPrices_.rbegin()->second;

    view.showMessage("Owner: " + getOwnerUsername() + "\n");
    view.showMessage("Jumlah railroad: " + std::to_string(count) + "\n");
    view.showMessage("Sewa: M" + std::to_string(rent) + "\n");

    if (p.getBalance() < rent) {
        return OnLandResult::TriggerAuction;
    }

    p.deductMoney(rent);
    if (owner) owner->addMoney(rent);

    return OnLandResult::Done;
}

// getter n setter
void RailRoadTile::setRailRoadRentPrices(const std::map<int, int>& prices) {
    railRoadRentPrices_ = prices;
}

const std::map<int, int>& RailRoadTile::getRailRoadRentPrices()
{
    return railRoadRentPrices_;
}
