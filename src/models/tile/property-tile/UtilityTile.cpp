#include "models/tile/property_tile/UtilityTile.hpp"

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

OnLandResult UtilityTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) 
{
    (void) command;

    view.showMessage("Kamu mendarat di " + getTileName() + "!\n");

    // belum dimiliki -> otomatis dimiliki 
    if (getPropertyStatus() == BANK || getOwnerUsername() == "BANK") {
        setOwnerUsername(p.getUsername());
        setPropertyStatus(OWNED);
        incrementOwnerCount(p.getUsername());

        view.showMessage("Belum ada yang menginjaknya duluan, " + getTileName() + " kini menjadi milikmu!\n");
        view.showMessage("---\n");
        return OnLandResult::Done;
    }

    // jika sedang digadaikan
    if (getPropertyStatus() == MORTGAGED) {
        view.showMessage("Utility ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.\n");
        view.showMessage("---\n");
        return OnLandResult::Done;
    }

    // kalau milik sendiri
    if (getOwnerUsername() == p.getUsername()) {
        view.showMessage("Utility ini adalah milikmu sendiri.\n");
        view.showMessage("---\n");
        return OnLandResult::Done;
    }

    int ownerCount = getOwnerCount(getOwnerUsername());
    if (ownerCount < 1) ownerCount = 1;

    int factor = 4;
    auto it = utilityFactor_.find(ownerCount);
    if (it != utilityFactor_.end()) {
        factor = it->second;
    } else if (!utilityFactor_.empty()) {
        factor = utilityFactor_.rbegin()->second;
    }

    int diceTotal = p.getLastDiceTotal();
    int rent = diceTotal * factor;

    view.showMessage("Kamu mendarat di " + getTileName() + ", milik " + getOwnerUsername() + "!\n");
    view.showMessage("Total dadu    : " + std::to_string(diceTotal) + "\n");
    view.showMessage("Faktor utility: " + std::to_string(factor) + "\n");
    view.showMessage("Sewa          : M" + std::to_string(rent) + "\n");

    int oldBalance = p.getBalance();
    if (oldBalance < rent) {
        view.showMessage("Kamu tidak mampu membayar sewa penuh! (M" + std::to_string(rent) + ")\n");
        view.showMessage("Uang kamu saat ini: M" + std::to_string(oldBalance) + "\n");
        view.showMessage("---\n");
        return OnLandResult::TriggerBankruptcyAuction;
    }

    p.deductMoney(rent);
    view.showMessage("Uang kamu: M" + std::to_string(oldBalance) + " -> M" + std::to_string(p.getBalance()) + "\n");
    view.showMessage("---\n");

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

void UtilityTile::incrementOwnerCount(const std::string& username) 
{
    if (username != "BANK") {
        ownerUtilityCount_[username]++;
    }
}

void UtilityTile::decrementOwnerCount(const std::string& username) 
{
    if (username != "BANK" && ownerUtilityCount_.count(username)) {
        ownerUtilityCount_[username]--;
        if (ownerUtilityCount_[username] <= 0) {
            ownerUtilityCount_.erase(username);
        }
    }
}

int UtilityTile::getOwnerCount(const std::string& username) 
{
    auto it = ownerUtilityCount_.find(username);
    if (it == ownerUtilityCount_.end()) return 0;
    return it->second;
}