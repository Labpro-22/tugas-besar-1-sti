#include "models/tile/PLNTile.hpp"

PLNTile::PLNTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : UtilityTile(tileID, letterCode, tileName, colourBlock, purchasePrice, mortgageValue) {}

PLNTile::~PLNTile() {}

void PLNTile::onLand(Player* player)
{
    if (player != nullptr)
    {
        executeTile(*player);
    }
}

void PLNTile::executeTile(Player& p)
{
    std::cout << "[Landed] Menginjak petak PLN. Siapkan biaya listrik!" << std::endl;
    UtilityTile::executeTile(p);
}
