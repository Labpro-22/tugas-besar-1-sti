#include "controllers/Auction.hpp"
#include <sstream>

Auction::Auction(std::vector<std::unique_ptr<Player>>& players,
                 GameViewInterface& view)
    : players_(players), view_(view), property_(nullptr), triggerPlayer_(nullptr), cause_(AuctionCause::PROPERTY_DECLINED), state_(AuctionState::NOT_STARTED), currentParticipantIndex_(0), highestBid_(0), highestBidder_(nullptr), passesCount_(0), hasAnyBid_(false), finished_(false) {}

Auction::~Auction() = default;

void Auction::reset() {
    property_ = nullptr;
    triggerPlayer_ = nullptr;
    participants_.clear();
    currentParticipantIndex_ = 0;
    highestBid_ = 0;
    highestBidder_ = nullptr;
    passesCount_ = 0;
    hasAnyBid_ = false;
    finished_ = false;
    state_ = AuctionState::NOT_STARTED;
}

void Auction::start(Player& triggerPlayer, PropertyTile& propertyTile, AuctionCause cause) {
    reset();

    triggerPlayer_ = &triggerPlayer;
    property_ = &propertyTile;
    cause_ = cause;
    state_ = AuctionState::WAITING_FOR_ACTION;

    buildParticipants();

    view_.showMessage("Properti " + property_->getTileName() +
                      " (" + property_->getLetterCode() + ") akan dilelang!\n");

    if (triggerPlayer_ != nullptr) {
        view_.showMessage("Urutan lelang dimulai dari pemain setelah " +
                          triggerPlayer_->getUsername() + ".\n");
    }

    if (participants_.empty()) {
        view_.showMessage("Tidak ada peserta valid untuk lelang.\n");
        finished_ = true;
        state_ = AuctionState::FINISHED;

        // fallback aman: property tetap milik BANK
        if (property_ != nullptr) {
            property_->resetAfterBankruptcyToBank();
        }
        return;
    }
}

void Auction::buildParticipants() {
    participants_.clear();

    int triggerIdx = findTriggerPlayerIndex();
    if (triggerIdx == -1) {
        return;
    }

    int n = static_cast<int>(players_.size());

    for (int step = 1; step <= n; step++) {
        int idx = (triggerIdx + step) % n;
        Player* candidate = players_[idx].get();

        if (!isValidParticipant(candidate)) {
            continue;
        }

        // kalau trigger player bangkrut ke bank, dia tidak ikut auction
        if (candidate == triggerPlayer_ &&
            cause_ == AuctionCause::BANKRUPTCY_TO_BANK &&
            triggerPlayer_ != nullptr &&
            triggerPlayer_->isBankrupt()) {
            continue;
        }

        participants_.push_back(candidate);
    }
}

bool Auction::isValidParticipant(const Player* player) const {
    if (player == nullptr) {
        return false;
    }
    if (player->isBankrupt()) {
        return false;
    }
    return true;
}

int Auction::findTriggerPlayerIndex() const {
    if (triggerPlayer_ == nullptr) {
        return -1;
    }
    for (size_t i = 0; i < players_.size(); i++) {
        if (players_[i].get() == triggerPlayer_) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int Auction::getMinBid() const {
    if (!hasAnyBid_) {
        return 0;
    }
    return highestBid_ + 1;
}

bool Auction::isForcedBidTurn() const {
    if (participants_.empty()) {
        return false;
    }
    if (hasAnyBid_) {
        return false;
    }
    return passesCount_ >= static_cast<int>(participants_.size()) - 1;
}

void Auction::advanceTurn() {
    if (participants_.empty()) {
        return;
    }
    currentParticipantIndex_++;
    if (currentParticipantIndex_ >= static_cast<int>(participants_.size())) {
        currentParticipantIndex_ = 0;
    }
}

void Auction::givePropertyToWinner() {
    if (property_ == nullptr) {
        return;
    }
    if (highestBidder_ == nullptr) {
        // fallback kalau somehow tidak ada winner, property tetap BANK
        property_->resetAfterBankruptcyToBank();
        return;
    }
    highestBidder_->deductMoney(highestBid_);
    highestBidder_->addProperty(property_);
}

// kalau udah menang : pindahin property ke pemenang
void Auction::finalizeAuction() {
    if (finished_) {
        return;
    }
    givePropertyToWinner();
    finished_ = true;
    state_ = AuctionState::FINISHED;
    if (highestBidder_ != nullptr && property_ != nullptr) {
        view_.showMessage("Lelang selesai!\n");
        view_.showMessage("Pemenang: " + highestBidder_->getUsername() + "\n");
        view_.showMessage("Harga akhir: M" + std::to_string(highestBid_) + "\n");
        view_.showMessage("Properti " + property_->getTileName() +
                          " (" + property_->getLetterCode() + ") kini dimiliki " +
                          highestBidder_->getUsername() + ".\n");
    } else {
        view_.showMessage("Lelang selesai tanpa pemenang. Properti tetap menjadi milik BANK.\n");
    }
}

// kalau bisa pass, berarti gak wajib bid, berarti gak forced bid, berarti gak boleh pass kalau forced bid
bool Auction::canCurrentPlayerPass() const {
    if (state_ != AuctionState::WAITING_FOR_ACTION || finished_) {
        return false;
    }
    if (participants_.empty()) {
        return false;
    }
    return !isForcedBidTurn();
}

bool Auction::canCurrentPlayerBid(int amount) const {
    if (state_ != AuctionState::WAITING_FOR_ACTION || finished_) {
        return false;
    }
    Player* current = getCurrentPlayer();
    if (current == nullptr) {
        return false;
    }
    if (amount < getMinBid()) {
        return false;
    }
    if (amount > current->getBalance()) {
        return false;
    }
    return true;
}

void Auction::passCurrentPlayer() {
    if (state_ != AuctionState::WAITING_FOR_ACTION || finished_) {
        return;
    }
    Player* current = getCurrentPlayer();
    if (current == nullptr) {
        return;
    }
    if (!canCurrentPlayerPass()) {
        view_.showMessage("Pemain saat ini tidak boleh PASS. Wajib melakukan bid.\n");
        return;
    }
    view_.showMessage(current->getUsername() + " memilih PASS.\n");
    passesCount_++;
    // kalau sudah ada bidder dan semua pemain lain pass berturut-turut then selesai
    if (hasAnyBid_ &&
        passesCount_ >= static_cast<int>(participants_.size()) - 1) {
        finalizeAuction();
        return;
    }
    advanceTurn();
}

void Auction::bidCurrentPlayer(int amount) {
    if (state_ != AuctionState::WAITING_FOR_ACTION || finished_) {
        return;
    }
    Player* current = getCurrentPlayer();
    if (current == nullptr) {
        return;
    }
    if (!canCurrentPlayerBid(amount)) {
        view_.showMessage("Bid tidak valid.\n");
        return;
    }
    highestBid_ = amount;
    highestBidder_ = current;
    passesCount_ = 0;
    hasAnyBid_ = true;

    view_.showMessage("Penawaran tertinggi: M" + std::to_string(highestBid_) +
                      " (" + current->getUsername() + ")\n");
    advanceTurn();
}

void Auction::processBankruptcyToBank(Player& bankruptPlayer) {
    view_.showMessage(bankruptPlayer.getUsername() + " dinyatakan BANGKRUT kepada Bank!\n");

    int remainingMoney = bankruptPlayer.getBalance();
    if (remainingMoney > 0) {
        view_.showMessage("Uang sisa M" + std::to_string(remainingMoney) +
                          " diserahkan ke Bank.\n");
        bankruptPlayer.deductMoney(remainingMoney);
    }

    std::vector<PropertyTile*> properties = bankruptPlayer.getProperties();

    bankruptPlayer.setStatus(Player::PlayerStatus::BANKRUPT);

    if (properties.empty()) {
        view_.showMessage("Pemain tidak memiliki properti untuk dilelang.\n");
        return;
    }

    view_.showMessage("Seluruh properti dikembalikan ke Bank.\n");

    for (size_t i = 0; i < properties.size(); i++) {
        PropertyTile* property = properties[i];
        if (property == nullptr) {
            continue;
        }

        bankruptPlayer.removeProperty(property);
        property->resetAfterBankruptcyToBank();
    }

    view_.showMessage("Properti siap dilelang satu per satu.\n");

    if (!properties.empty() && properties[0] != nullptr) {
        start(bankruptPlayer, *properties[0], AuctionCause::BANKRUPTCY_TO_BANK);
    }
}

bool Auction::isStarted() const {
    return state_ != AuctionState::NOT_STARTED;
}

bool Auction::isFinished() const {
    return finished_;
}

Auction::AuctionState Auction::getState() const {
    return state_;
}

Auction::AuctionCause Auction::getCause() const {
    return cause_;
}

PropertyTile* Auction::getProperty() const {
    return property_;
}

Player* Auction::getTriggerPlayer() const {
    return triggerPlayer_;
}

Player* Auction::getCurrentPlayer() const {
    if (participants_.empty()) {
        return nullptr;
    }
    if (currentParticipantIndex_ < 0 ||
        currentParticipantIndex_ >= static_cast<int>(participants_.size())) {
        return nullptr;
    }
    return participants_[currentParticipantIndex_];
}

Player* Auction::getHighestBidder() const {
    return highestBidder_;
}

int Auction::getHighestBid() const {
    return highestBid_;
}

int Auction::getCurrentParticipantIndex() const {
    return currentParticipantIndex_;
}

int Auction::getpassesCount() const {
    return passesCount_;
}

int Auction::getParticipantSize() const {
    return static_cast<int>(participants_.size());
}

const std::vector<Player*>& Auction::getParticipants() const {
    return participants_;
}

std::string Auction::getStatusText() const {
    std::ostringstream oss;

    oss << "=== AUCTION STATUS ===\n";

    if (property_ != nullptr) {
        oss << "Property   : " << property_->getTileName()
            << " (" << property_->getLetterCode() << ")\n";
    } else {
        oss << "Property   : -\n";
    }

    if (triggerPlayer_ != nullptr) {
        oss << "Triggered by: " << triggerPlayer_->getUsername() << "\n";
    } else {
        oss << "Triggered by: -\n";
    }

    oss << "State      : ";
    if (state_ == AuctionState::NOT_STARTED) {
        oss << "NOT_STARTED\n";
    } else if (state_ == AuctionState::WAITING_FOR_ACTION) {
        oss << "WAITING_FOR_ACTION\n";
    } else {
        oss << "FINISHED\n";
    }

    Player* current = getCurrentPlayer();
    if (current != nullptr) {
        oss << "Current turn: " << current->getUsername() << "\n";
    } else {
        oss << "Current turn: -\n";
    }

    if (highestBidder_ != nullptr) {
        oss << "Highest bid: M" << highestBid_
            << " by " << highestBidder_->getUsername() << "\n";
    } else {
        oss << "Highest bid: belum ada\n";
    }

    oss << "Participants: ";
    for (size_t i = 0; i < participants_.size(); i++) {
        oss << participants_[i]->getUsername();
        if (i + 1 < participants_.size()) {
            oss << ", ";
        }
    }
    oss << "\n";

    if (isForcedBidTurn()) {
        oss << "Forced bid : YES\n";
    } else {
        oss << "Forced bid : NO\n";
    }

    return oss.str();
}