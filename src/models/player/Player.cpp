#include "models/player/Player.hpp"

Player::Player(std::string username, int initialBalance) :
    id_(countPlayer++), username_(username),
    balance_(initialBalance), position_(0),
    status_(PlayerStatus::ACTIVE), doubleRollCount_(0),
    countJail_(0), playerTurn_(0) {}

Player::~Player() = default;

// SET TURN
void Player::setTurn(int turn) {
    playerTurn_ = turn;
}
// MONEY RELATED
int Player::getBalance() const {
    return balance_;
}

void Player::addMoney(int amount) {
    balance_ += amount;
}

void Player::deductMoney(int amount) {
    balance_ -= amount;
}

// STATUS RELATED
Player::PlayerStatus Player::getStatus() const {
    return status_;
}

void Player::setStatus(Player::PlayerStatus status) {
    status_ = status;
}

// MOVEMENT OR POSITION RELATED
int Player::getPosition() const {
    return position_;
}
void Player::setPosition(int pos) {
    position_ = pos;
}

int Player::move(int displacement) {
    return this->position_ + displacement;
}

void Player::incrementDoubleCount() {
    doubleRollCount_++;
}

bool Player::safeToGetMoreDouble() const {
    return doubleRollCount_ + 1 < 3;
}

// Jail related
bool Player::isInJail() const {
    return status_ == PlayerStatus::JAILED;
}

bool Player::thisTurnAutoFreeFromJail() const {
    return countJail_ + 1 == 3;
}

void Player::incrementJailTurn() {
    countJail_++;
}

int Player::getCountJail() const {
    return countJail_;
}

void Player::resetJailTurn() {
    countJail_ = 0;
}

// TAX RELATED
int Player::getTotalPropertyValue() const {
    return inventory_.countAllPropertyValueBasedOnPurchasePrice();
}

int Player::getTotalBuildingValue() const {
    return inventory_.countAllBuildingsBasedOnPurchasePrice();
}
// PPH related
bool Player::isBankrupt() const {
    return status_ == PlayerStatus::BANKRUPT;
}

// Inventory related

std::vector<SkillCard*> Player::getSkillCards(){
    return inventory_.getSkillCards();
}
std::vector<PropertyTile*> Player::getProperties(){
    return inventory_.getProperties();

}