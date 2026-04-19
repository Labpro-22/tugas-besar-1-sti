#include "models/tile/PropertyTile.hpp"

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

void PropertyTile::executeTile(Player& p)
{
    (void)p;
    std::cout << "[Landed] Anda tiba di properti: " << tileName_ << std::endl;
}
