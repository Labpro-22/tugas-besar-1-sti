#include "models/tile/property_tile/StreetTile.hpp"
#include "models/player/Player.hpp"
#include "utils/Formatter.hpp"

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
    view.showMessage("Kamu mendarat di " + getTileName() + " (" + getLetterCode() + ")");

    // === MILIK SENDIRI ===
    if (getOwnerUsername() == p.getUsername()) {
        view.showMessage("!\nKamu memiliki properti ini.\n");
        return OnLandResult::Done;
    }
    
    // === MILIK ORANG LAIN ===
    if (!isOwnedByBank()) {
        if (isMortgaged()) {
            view.showMessage(", milik " + getOwnerUsername() + ".\n");
            view.showMessage("Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.\n");
            return OnLandResult::Done;
        }
        // artinya player lain
        return OnLandResult::TriggerTryToPayRent;
    }

    // === BELUM DIMILIKI (PROSES BELI) ===
    view.showMessage("!\n");
    view.cetakAkta(getLetterCode(), false);
    view.showMessage("Uang kamu saat ini: " + Formatter::formattingMoney(p.getBalance()) + "\n");
    if (p.getBalance() < getPurchasePrice()) {
        view.showMessage("Uangmu tidak cukup.\n");
        view.showMessage("Properti ini akan masuk ke sistem lelang...\n");
        return OnLandResult::TriggerAuction;
    }

    view.showMessage("Apakah kamu ingin membeli properti ini seharga " + Formatter::formattingMoney(getPurchasePrice()) + "?\n");
    bool wantToBuy = command.askWantToBuyProperty();
    
    if (!wantToBuy) {
        view.showMessage("Properti ini akan masuk ke sistem lelang...\n");
        return OnLandResult::TriggerAuction;
    }

    p.deductMoney(getPurchasePrice());
    setOwnerUsername(p.getUsername());
    setPropertyStatus(OWNED); 
    p.addProperty(this);

    view.showMessage(getTileName() + " kini menjadi milikmu!\n");
    view.showMessage("Uang tersisa: " + Formatter::formattingMoney(p.getBalance()) + "\n");

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