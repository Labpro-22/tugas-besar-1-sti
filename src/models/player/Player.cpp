#include "models/player/Player.hpp"
#include "models/card/skillcard/SkillCard.hpp"
#include "models/exception/InvariantViolationException/InvariantViolationException.hpp"

namespace {
std::vector<Player*> g_allPlayers;
}

Player::Player(std::string username, int initialBalance) :
    id_(countPlayer++), username_(username),
    balance_(initialBalance), position_(0),
    status_(PlayerStatus::ACTIVE), doubleRollCount_(0),
    countJail_(0), playerTurn_(0), shieldTurns_(0),
    discountPercent_(0), discountTurns_(0)
{
    g_allPlayers.push_back(this);
    for (Player* player : g_allPlayers) {
        if (player != nullptr) {
            player->setAllPlayersContext(g_allPlayers);
        }
    }
}

Player::~Player()
{
    auto it = std::find(g_allPlayers.begin(), g_allPlayers.end(), this);
    if (it != g_allPlayers.end()) {
        g_allPlayers.erase(it);
    }
    for (Player* player : g_allPlayers) {
        if (player != nullptr) {
            player->setAllPlayersContext(g_allPlayers);
        }
    }
}

int Player::getID() const {
    return id_;
}

std::string Player::getUsername() const {
    return username_;
}

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
    if (balance_ - amount < 0) {
        throw InsufficientFundsException(101, "Saldo tidak cukup!\n");
    }
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

int Player::notViolatingDoubleRollCount() const {
    return doubleRollCount_ < 3;
}

// Jail related
bool Player::isInJail() const {
    return status_ == PlayerStatus::JAILED;
}

bool Player::thisTurnAutoFreeFromJail() const {
    return countJail_ == 0;
}

void Player::decrementJailTurn() {
    countJail_--;
}

int Player::getCountJail() const {
    return countJail_;
}

void Player::resetJailTurn() {
    countJail_ = 0;
}

void Player::setJailTurn(int count) {
    countJail_ = count;
}

void Player::leaveJail() {
    status_ = PlayerStatus::ACTIVE;
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

void Player::decreaseShieldCardTurn() {
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

std::vector<SkillCard*> Player::getSkillCards() {
    return inventory_.getSkillCards();
}
std::vector<PropertyTile*> Player::getProperties() {
    return inventory_.getProperties();
}

void Player::addProperty(PropertyTile* propertyTile) {
    if (propertyTile == nullptr) return;

    inventory_.addProperty(propertyTile);
    propertyTile->setOwnerUsername(username_);
    propertyTile->setPropertyStatus(OWNED);
}

void Player::removeProperty(PropertyTile* propertyTile) {
    if (propertyTile == nullptr) return;

    inventory_.removeProperty(propertyTile);
}

bool Player::ownsProperty(PropertyTile* propertyTile) const {
    if (propertyTile == nullptr) return false;

    std::vector<PropertyTile*> properties = inventory_.getProperties();
    for (size_t i = 0; i < properties.size(); i++) {
        if (properties[i]->getTileID() == propertyTile->getTileID()) {
            return true;
        }
    }
    return false;
}

bool Player::hasProperty(std::string tileCode) {
    std::vector<PropertyTile*> properties = inventory_.getProperties();
    for (PropertyTile* p : properties) {
        if (p != nullptr && p->getLetterCode() == tileCode) {
            return true;
        }
    }
    return false;
}

PropertyTile& Player::getProperty(std::string code) {
    std::vector<PropertyTile*> properties = inventory_.getProperties();
    for (PropertyTile* p : properties) {
        if (p != nullptr && p->getLetterCode() == code) {
            return *p;
        }
    }
    throw InvariantViolationException(202, "Property not found: " + code);
}

Inventory& Player::getInventory() {
    return inventory_;
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

std::unique_ptr<SkillCard> Player::removeSkillCardAt(std::size_t idx) {
    return inventory_.removeSkillCardAt(idx);
}

void Player::setAllPlayersContext(const std::vector<Player*>& players)
{
    allPlayers_ = players;
}

const std::vector<Player*>& Player::getAllPlayers() const {
    return allPlayers_;
}

int Player::countRailroad() const {
    return inventory_.countRailRoads();
}

int Player::countUtilities() const {
    return inventory_.countUtilities();
}

std::vector<PropertyTile*> Player::getMortgagedProperties() const {
    return inventory_.getMortgagedProperties();
}

int Player::getPropertyCount() const {
    return inventory_.countProperty();
}

int Player::getCardCount() const {
    return inventory_.countCard();
}

std::map<std::string, std::vector<PropertyTile*>> Player::getCompleteColourGroups(std::map<std::string, size_t> countTilesForEachColourBlock) {
    return inventory_.getCompleteColourGroups(countTilesForEachColourBlock);
}

std::map<std::string, std::vector<PropertyTile*>> Player::getOwnedPropertiesGroupByColourGroups() {
    return inventory_.getOwnedPropertiesGroupByColourGroups();
}

void Player::resetCountDouble() {
    doubleRollCount_ = 0;
}

bool Player::operator>(const Player& other) const {
    if (getBalance() > other.getBalance()) {
        return true;
    }

    if (getPropertyCount() > other.getPropertyCount()) {
        return true;
    }

    if (getCardCount() > other.getCardCount()) {
        return true;
    }
    return false;
}

bool Player::operator==(const Player& other) const {
    return getBalance() == other.getBalance() &&
        getPropertyCount() == other.getPropertyCount() &&
        getCardCount() == other.getCardCount();
}
