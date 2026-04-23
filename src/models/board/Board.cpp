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
