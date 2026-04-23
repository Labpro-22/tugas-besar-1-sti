#include "models/tile/property_tile/StreetTile.hpp"

StreetTile::StreetTile(int tileID, std::string letterCode,
    std::string tileName, std::string colourBlock,
    int purchasePrice, int mortgageValue,
    std::map<int, int> rentPricePerLevel,
    std::map<int, int> buildPrice)
        : PropertyTile(tileID, letterCode, tileName,
            colourBlock, purchasePrice, mortgageValue),
            rentPricePerLevel_(rentPricePerLevel),
            buildPrice_(buildPrice) {}

OnLandResult StreetTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    view.showMessage("Halo kamu ada di street tile");

    // milik sendiri
    if (getOwnerUsername() == p.getUsername()) {
        return OnLandResult::Done;
    }
    
    if (getOwnerUsername() != "BANK") {
        // artinya player lain
        if (p.getBalance() < calculateRentPrice()) {
            return OnLandResult::TriggerBankruptcyFlow;
        }
    }

    // punya opsi buat beli
    if (p.getBalance() < getPurchasePrice()) {
        return OnLandResult::TriggerAuction;
    }

    bool wantToBuy = command.askWantToBuyProperty();
    
    if (!command.askWantToBuyProperty()) {
        return OnLandResult::TriggerAuction;
    }

    // calculate
    p.deductMoney(getPurchasePrice());
    setOwnerUsername(p.getUsername());

    // proses beli
    view.showMessage("Pembelian berhasil dilakukan!\n");
    return OnLandResult::Done;
}

std::map<int, int> StreetTile::getBuildPrice() const {
    return buildPrice_;
}
