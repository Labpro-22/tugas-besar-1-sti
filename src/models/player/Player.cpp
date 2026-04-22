#include "models/player/Player.hpp"

Player::Player(std::string username, int initialBalance) :
    id_(countPlayer++), username_(username),
    balance_(initialBalance), position_(0),
    status_(PlayerStatus::ACTIVE), doubleRollCount_(0),
    countJail_(0), playerTurn_(0), shieldTurns_(0),
    discountPercent_(0), discountTurns_(0) {}

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
    if (amount > 0 && isShielded()) {
        return;
    }

    if (amount > 0 && isDiscounted()) {
        amount -= (amount * discountPercent_) / 100;
    }

    balance_ -= amount;
}

// STATUS RELATED
Player::PlayerStatus Player::getStatus() const {
    return status_;
}

void Player::setStatus(Player::PlayerStatus status) {
    if (status == PlayerStatus::JAILED && isShielded()) {
        return;
    }
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

void Player::activateShield(int turns) {
    if (turns <= 0) {
        return;
    }
    shieldTurns_ = turns;
}

bool Player::isShielded() const {
    return shieldTurns_ > 0;
}

void Player::consumeShield() {
    if (shieldTurns_ > 0) {
        --shieldTurns_;
    }
}

void Player::activateDiscount(int percent, int turns) {
    if (percent <= 0 || turns <= 0) {
        return;
    }

    discountPercent_ = percent;
    discountTurns_ = turns;
}

bool Player::isDiscounted() const {
    return discountTurns_ > 0 && discountPercent_ > 0;
}

int Player::getDiscountPercent() const {
    return discountPercent_;
}

void Player::consumeDiscount() {
    if (discountTurns_ > 0) {
        --discountTurns_;
    }

    if (discountTurns_ == 0) {
        discountPercent_ = 0;
    }
}

// Inventory related

const std::vector<std::unique_ptr<SkillCard>>& Player::getSkillCards() const {
    return inventory_.getSkillCards();
}
std::vector<std::reference_wrapper<PropertyTile>> Player::getProperties() {
    return inventory_.getProperties();

}

void Player::addSkillCard(std::unique_ptr<SkillCard> skillCard) {
    inventory_.addSkillCards(std::move(skillCard));
}

std::size_t Player::getSkillCardCount() const {
    return inventory_.getSkillCardCount();
}

const SkillCard* Player::getSkillCardAt(std::size_t idx) const {
    return inventory_.getSkillCardAt(idx);
}

std::unique_ptr<SkillCard> Player::takeSkillCard(std::size_t idx) {
    return inventory_.takeSkillCard(idx);
}