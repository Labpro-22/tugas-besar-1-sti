#include "models/tile/property_tile/PropertyTile.hpp"

PropertyTile::PropertyTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : Tile(tileID, letterCode, tileName, colourBlock),
      ownerUsername_("BANK"),
      festivalMultiplier_(1),
      festivalDuration_(0),
      propertyStatus_(BANK),
      level_(0),
      mortgageValue_(mortgageValue),
      purchasePrice_(purchasePrice) {}

PropertyTile::~PropertyTile() {}

int PropertyTile::getPurchasePrice() const {
    return purchasePrice_;
}

int PropertyTile::getMortgageValue() const {
    return mortgageValue_;
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