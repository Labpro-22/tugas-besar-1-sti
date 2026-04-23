#include "models/tile/property_tile/RailRoadTile.hpp"

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

    // belum dimiliki siapa-siapa -> otomatis dimiliki
    if (getPropertyStatus() == BANK || getOwnerUsername() == "BANK") {
        setOwnerUsername(p.getUsername());
        setPropertyStatus(OWNED);
        incrementOwnerCount(p.getUsername());

        view.showMessage("Belum ada yang menginjaknya duluan, stasiun ini kini menjadi milikmu!\n");
        view.showMessage("---\n");
        return OnLandResult::Done;
    }

    // jika sedang digadaikan
    if (getPropertyStatus() == MORTGAGED) {
        view.showMessage("Stasiun ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.\n");
        view.showMessage("---\n");
        return OnLandResult::Done;
    }

    // kalau mendarat di milik sendiri
    if (getOwnerUsername() == p.getUsername()) {
        view.showMessage("Stasiun ini adalah milikmu sendiri.\n");
        view.showMessage("---\n");
        return OnLandResult::Done;
    }

    int ownerCount = getOwnerCount(getOwnerUsername());
    if (ownerCount < 1) ownerCount = 1;

    int rent = 25;
    auto it = railRoadRentPrices_.find(ownerCount);
    if (it != railRoadRentPrices_.end()) {
        rent = it->second;
    } else if (!railRoadRentPrices_.empty()) {
        rent = railRoadRentPrices_.rbegin()->second;
    }

    view.showMessage("Kamu mendarat di " + getTileName() + " (" + getLetterCode() + "), milik " + getOwnerUsername() + "!\n");
    view.showMessage("Sewa         : M" + std::to_string(rent) + "\n");

    int oldBalance = p.getBalance();
    if (oldBalance < rent) {
        view.showMessage("Kamu tidak mampu membayar sewa penuh! (M" + std::to_string(rent) + ")\n");
        view.showMessage("Uang kamu saat ini: M" + std::to_string(oldBalance) + "\n");
        view.showMessage("---\n");
        return OnLandResult::TriggerAuction;
    }

    p.deductMoney(rent);
    view.showMessage("Uang kamu: M" + std::to_string(oldBalance) + " -> M" + std::to_string(p.getBalance()) + "\n");
    view.showMessage("---\n");

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

void RailRoadTile::incrementOwnerCount(const std::string& username) {
    if (username != "BANK") {
        ownerRailroadCount_[username]++;
    }
}

void RailRoadTile::decrementOwnerCount(const std::string& username) {
    if (username != "BANK" && ownerRailroadCount_.count(username)) {
        ownerRailroadCount_[username]--;
        if (ownerRailroadCount_[username] <= 0) {
            ownerRailroadCount_.erase(username);
        }
    }
}

int RailRoadTile::getOwnerCount(const std::string& username) {
    auto it = ownerRailroadCount_.find(username);
    if (it == ownerRailroadCount_.end()) return 0;
    return it->second;
}
