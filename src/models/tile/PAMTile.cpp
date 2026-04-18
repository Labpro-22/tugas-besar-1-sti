#include "models/tile/PAMTile.hpp"

PAMTile::PAMTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : UtilityTile(tileID, letterCode, tileName, colourBlock, purchasePrice, mortgageValue) {}

PAMTile::~PAMTile() {}

void PAMTile::onLand(Player* player)
{
    if (player != nullptr)
    {
        executeTile(*player);
    }
}

void PAMTile::executeTile(Player& p)
{
    std::cout << "[Landed] Menginjak petak PAM. Tagihan air menunggu." << std::endl;
    UtilityTile::executeTile(p);
}
