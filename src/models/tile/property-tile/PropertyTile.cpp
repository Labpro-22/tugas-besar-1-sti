#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/tile/property_tile/StreetTile.hpp"
PropertyTile::PropertyTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : Tile(tileID, letterCode, tileName, colourBlock),
      ownerUsername_("BANK"),
      festivalMultiplier_(1),
      festivalDuration_(0),
      propertyStatus_(BANK),
      level_(0),
      mortgageValue_(mortgageValue),
      purchasePrice_(purchasePrice) {}

int PropertyTile::getPurchasePrice() const {
    return purchasePrice_;
}

int PropertyTile::getMortgageValue() const {
    return mortgageValue_;
}

std::string PropertyTile::getColourBlock() const {
    return colourBlock_;
}

int PropertyTile::getLevel() const {
    return level_;
}

void PropertyTile::setLevel(int level) {
    level_ = level;
}

std::string PropertyTile::getOwnerUsername() const {
    return ownerUsername_;
}

void PropertyTile::setOwnerUsername(const std::string& ownerUsername) {
    ownerUsername_ = ownerUsername;
}

PropertyStatus PropertyTile::getPropertyStatus() const {
    return propertyStatus_;
}

void PropertyTile::setPropertyStatus(PropertyStatus status) {
    propertyStatus_ = status;
}

int PropertyTile::getFestivalMultiplier() const {
    return festivalMultiplier_;
}

void PropertyTile::setFestivalMultiplier(int multiplier) {
    festivalMultiplier_ = multiplier;
}

int PropertyTile::getFestivalDuration() const {
    return festivalDuration_;
}

void PropertyTile::setFestivalDuration(int duration) {
    festivalDuration_ = duration;
}

bool PropertyTile::isOwnedByBank() const {
    return ownerUsername_ == "BANK" && propertyStatus_ == BANK;
}

bool PropertyTile::isOwnedBy(const std::string& username) const {
    return ownerUsername_ == username;
}

void PropertyTile::resetFestivalEffect() {
    festivalMultiplier_ = 1;
    festivalDuration_ = 0;
}

void PropertyTile::resetAfterBankruptcyToBank() {
    ownerUsername_ = "BANK";
    propertyStatus_ = BANK;
    festivalMultiplier_ = 1;
    festivalDuration_ = 0;
    level_ = 0;
}

bool PropertyTile::isMortgaged() const {
    return getPropertyStatus() == PropertyStatus::MORTGAGED;
}

bool PropertyTile::isOwned() const {
    return propertyStatus_ == OWNED;
}

bool PropertyTile::hasBuilding() const {
    return level_ > 0;
}

void PropertyTile::setToMortgaged() {
    propertyStatus_ = MORTGAGED;
}

int PropertyTile::getSellingPrice() const {
    return mortgageValue_;
}

int PropertyTile::canDoubleFestival() const {
    return !alreadyMaxMultiplier() && festivalDuration_ > 0;
}

bool PropertyTile::festivalActive() {
    return festivalMultiplier_ > 1 && festivalDuration_ > 0;
}

bool PropertyTile::alreadyMaxMultiplier() const {
    return festivalMultiplier_ >= 8;
}

int PropertyTile::doubleTheMultiplier() {
    if (!alreadyMaxMultiplier()) {
        festivalMultiplier_ *= 2;
        if (festivalMultiplier_ > 8) {
            festivalMultiplier_ = 8;
        }
    }
    return festivalMultiplier_;
}

int PropertyTile::resetTurnTo3() {
    festivalDuration_ = 3;
    return festivalDuration_;
}

int PropertyTile::sellAllBuildings() const {
    const StreetTile* street = dynamic_cast<const StreetTile*>(this);
    if (street == nullptr || level_ <= 0) {
        return 0;
    }

    const std::map<int, int> buildPrice = street->getBuildPrice();
    int total = 0;
    for (int level = 1; level <= level_; ++level) {
        auto it = buildPrice.find(level);
        if (it != buildPrice.end()) {
            total += it->second / 2;
        }
    }
    return total;
}
std::string PropertyTile::propertyStatusToText() const{
    if (propertyStatus_ == BANK) {return "BANK";}
    if (propertyStatus_== OWNED) {return "OWNED";}
    if (propertyStatus_ == MORTGAGED) {return "MORTGAGED";}
    return "UNKNOWN";
}

std::string PropertyTile::buildingLevelToText() const {
    if (level_ <= 0) {return "-";}
    if (level_ >= 5) {return "Hotel";}
    return std::to_string(level_) + " rumah";
}