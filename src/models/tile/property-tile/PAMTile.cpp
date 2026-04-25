#include "models/tile/property_tile/PAMTile.hpp"

PAMTile::PAMTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : UtilityTile(tileID, letterCode, tileName, colourBlock, purchasePrice, mortgageValue) {}

OnLandResult PAMTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    return UtilityTile::onLand(p, command, view);
}
