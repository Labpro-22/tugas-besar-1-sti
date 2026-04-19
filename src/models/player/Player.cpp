#include "models/player/Player.hpp"

Player::Player(std::string username, int initialBalance, int playerTurn) :
    id_(countPlayer++), username_(username),
    balance_(initialBalance), position(0),
    status_(PlayerStatus::ACTIVE), doubleRollCount_(0),
    countJail_(0), playerTurn_(playerTurn) {}

Player::~Player() = default;

bool Player::thisTurnFreeFromJail() const {
    return countJail_ + 1 == 3;
}