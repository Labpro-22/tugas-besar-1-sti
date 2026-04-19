#include "../../../include/models/tile/Tile.hpp"

Tile::Tile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
    : tileID_(tileID), letterCode_(letterCode), tileName_(tileName), colourBlock_(colourBlock) {
}

Tile::~Tile() {}

void Tile::getDetail() const {
    // Bisa di-override sama child-nya
    std::cout << "[" << letterCode_ << "] " << tileName_ << " (Blok Warna: " << colourBlock_ << ")\n";
}

int Tile::getTileID() const {
    return tileID_;
}

std::string Tile::getLetterCode() const {
    return letterCode_;
}

std::string Tile::getTileName() const {
    return tileName_;
}

std::string Tile::getColourBlock() const {
    return colourBlock_;
}