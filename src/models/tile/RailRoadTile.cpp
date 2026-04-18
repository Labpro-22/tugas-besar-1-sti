#include "models/tile/RailRoadTile.hpp"

RailRoadTile::RailRoadTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : PropertyTile(tileID, letterCode, tileName, colourBlock, purchasePrice, mortgageValue) {}

RailRoadTile::~RailRoadTile() {}

void RailRoadTile::onLand(Player* player)
{
    if (player != nullptr)
    {
        executeTile(*player);
    }
}

void RailRoadTile::executeTile(Player& p)
{
    (void)p;
    std::cout << "[Landed] Anda tiba di stasiun: " << tileName_ << std::endl;
}
