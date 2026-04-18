#include "models/tile/Tile.hpp"

Tile::Tile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
    : tileID_(tileID),
      letterCode_(letterCode),
      tileName_(tileName),
      colourBlock_(colourBlock) {}

Tile::~Tile() {}