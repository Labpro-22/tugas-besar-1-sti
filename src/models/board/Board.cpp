#include "models/board/Board.hpp"

Board::Board(int size) : size_(size) {}

Board::~Board() = default;

void Board::addTile(std::unique_ptr<Tile> tile) {
    tiles_.push_back(std::move(tile));
}

Tile& Board::moveToNextTile(int distance) {
    return *tiles_.at(distance % size_);
}

Tile& Board::getCurrentTile(int id) {
    return *tiles_.at(id);
}

int Board::getSize() const {
    return size_;
}

int Board::getJailPosition() const {
    return jailPosition_;
}

void Board::setJailPosition(int pos) {
    jailPosition_ = pos;
}

int Board::getStartPosition() const {
    return startPosition_;
}

void Board::setStartPosition(int pos) {
    startPosition_ = pos;
}

Tile& Board::getTileByCode(const std::string& code) {
    for (auto& tile : tiles_) {
        if (tile->getLetterCode() == code) {
            return *tile;
        }
    }
    throw InvariantViolationException(203, "Error: Petak dengan kode " + code + " tidak ditemukan di Board!");
}

int Board::getTileIndexByCode(const std::string& code) const {
    for (int i = 0; i < (int)tiles_.size(); ++i) {
        if (tiles_[i]->getLetterCode() == code) {
            return i;
        }
    }
    return -1; 
}

bool Board::has(std::string code) {
    for (size_t i = 0; i < tiles_.size(); i++) {
        if (tiles_[i]->getLetterCode() == code) {
            return true;
        }
    }
    return false;
}
bool Board::isCompletedColourGroup(std::string ownerName, std::string colourBlock) {
    for (size_t i = 0; i < tiles_.size(); i++) {
        if (PropertyTile* propTile = dynamic_cast<PropertyTile*>(tiles_.at(i).get())) {
            if (propTile->getColourBlock() == colourBlock && (!propTile->isOwnedBy(ownerName) || propTile->isOwnedByBank())) {
                return false;
            }
        }
    }
    
    return true;
}

std::map<std::string, size_t> Board::getCountTilesForEachColourBlock() const {
    return countTilesForEachColourBlock_;
}