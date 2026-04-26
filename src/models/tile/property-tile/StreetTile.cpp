#include "models/tile/property_tile/StreetTile.hpp"
#include "models/player/Player.hpp"

StreetTile::StreetTile(int tileID, std::string letterCode,
    std::string tileName, std::string colourBlock,
    int purchasePrice, int mortgageValue,
    std::map<int, int> rentPricePerLevel,
    std::map<int, int> buildPrice)
        : PropertyTile(tileID, letterCode, tileName,
            colourBlock, purchasePrice, mortgageValue),
            buildPrice_(buildPrice),
            rentPricePerLevel_(rentPricePerLevel)
            {}

StreetTile::~StreetTile() = default;

OnLandResult StreetTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    view.showMessage("Halo kamu ada di street tile\n");
    if (isMortgaged()) {
        view.showMessage("Properti sedang di Mortgaged, tidak ada sewa\n");
        return OnLandResult::Done;
    }

    // milik sendiri
    if (getOwnerUsername() == p.getUsername()) {
        view.showMessage("Kamu mendarat di properti milikmu.\n");
        return OnLandResult::Done;
    }
    
    if (!isOwnedByBank()) {
        // artinya player lain
        return OnLandResult::TriggerTryToPayRent;
    }

    if (p.getBalance() < getPurchasePrice()) {
        return OnLandResult::TriggerAuction;
    }

    bool wantToBuy = command.askWantToBuyProperty();
    if (!wantToBuy) {
        return OnLandResult::TriggerAuction;
    }

    // calculate
    p.deductMoney(getPurchasePrice());
    setOwnerUsername(p.getUsername());

    // proses beli
    view.showMessage("Pembelian berhasil dilakukan!\n");
    return OnLandResult::Done;
}
//  Apabila pemain memonopoli seluruh Street dalam satu color 
//  group dan belum ada bangunan yang didirikan, sewa yang dikenakan 
//  adalah dua kali sewa dasar. Setelah bangunan mulai didirikan, 
//  sewa mengikuti tabel yang tertera pada Akta Kepemilikan, 
//  mulai dari 1 rumah hingga hotel. 
//  Jika efek Festival aktif pada properti tersebut, nilai sewa 
//  yang dikenakan adalah nilai sewa terkini setelah penggandaan
//  Festival diterapkan.

int StreetTile::calculateRentPrice(bool completedColourGroup) const {
    int rent = 0;
    auto it = rentPricePerLevel_.find(level_);
    int biayaSewa = it->second;
    if (level_ == 0) {
        if (completedColourGroup) {
            rent = biayaSewa * 2;
        } else {
            rent = biayaSewa;
        }
    } else {
        rent = biayaSewa;
    }
    return rent * festivalMultiplier_;
}


std::map<int, int> StreetTile::getBuildPrice() const {
    return buildPrice_;
}

void StreetTile::upgradeBuilding() {
    if (level_ + 1 > 4) {
        // throw
    }
    level_++;
}
int StreetTile::getBuildNextBuildingPrice() const { return 0; }

int StreetTile::getLevel() const {
    return PropertyTile::getLevel();
}

void StreetTile::setLevel(int level) {
    PropertyTile::setLevel(level);
}

int StreetTile::getRentAtLevel(int level) const {
    if (rentPricePerLevel_.count(level)) {
        return rentPricePerLevel_.at(level);
    }
    return -1;
}