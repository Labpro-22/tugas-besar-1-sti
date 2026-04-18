#include "models/tile/UtilityTile.hpp"

UtilityTile::UtilityTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : PropertyTile(tileID, letterCode, tileName, colourBlock, purchasePrice, mortgageValue) {}

UtilityTile::~UtilityTile() {}

void UtilityTile::executeTile(Player& p)
{
    (void)p;
    std::cout << "[Landed] Anda tiba di utilitas: " << tileName_ << std::endl;
}
