#include "controllers/GameController.hpp"
#include <utility>

GameController::GameController(std::vector<std::unique_ptr<Player>> players,
    Board& board, Dice& dice, GameViewInterface& view,
    CommandInterface& command, Deck<SkillCard>& specialCardDeck)
    : specialCardDeck_(specialCardDeck), board_(board), dice_(dice),
    view_(view), command_(command), players_(std::move(players)), auction_(players_, view_){}

GameController::~GameController() = default;

// TERKAIT LOGIC GAME SECARA UMUM =========================================================================================================
void GameController::playGame(int latestTurn, int maxTurn) {
    int i = latestTurn;
    while ((i < maxTurn || maxTurn == -1) && !hasSoleWinner()) {
        for (auto& player : players_) {
            if (!player->isBankrupt()) {
                processTurn(*player);
            }
        }
        i++;
    }
    decideWinner();
}

void GameController::decideWinner() const {
    std::vector<Player*> candidates;
    for (const auto& playerPtr : players_) {
        if (playerPtr != nullptr && !playerPtr->isBankrupt()) {
            candidates.push_back(playerPtr.get());
        }
    }
    if(hasSoleWinner()){
        for (const auto& playerPtr : players_) {
            if (!playerPtr->isBankrupt()) {
                view_.showMessage("Pemenang: " + playerPtr->getUsername() + "\n");
                view_.showMessage("Uang: M" + std::to_string(playerPtr->getBalance()) + "\n");
                return;
            }
        }
    }
    if (candidates.empty()) {
        view_.showMessage("Tidak ada pemenang karena semua pemain bangkrut.\n");
        return;
    }

    // uang terbanyak
    int maxBalance = candidates[0]->getBalance();
    for (Player* player : candidates) {
        if (player->getBalance() > maxBalance) {
            maxBalance = player->getBalance();
        }
    }
    // cari player yang balance nya = maxBalance
    std::vector<Player*> balanceWinners;
    for (Player* player : candidates) {
        if (player->getBalance() == maxBalance) {
            balanceWinners.push_back(player);
        }
    }
    // kalo cuma 1 yang uangnya paling banyak
    if (balanceWinners.size() == 1) {
        view_.showMessage("Pemenang: " + balanceWinners[0]->getUsername() + "\n");
        view_.showMessage("Uang: M" + std::to_string(balanceWinners[0]->getBalance()) + "\n");
        return;
    }

    // property terbanyak
    int maxPropertyCount = balanceWinners[0]->getPropertyCount();
    for (Player* player : balanceWinners) {
        if (player->getPropertyCount() > maxPropertyCount) {
            maxPropertyCount = player->getPropertyCount();
        }
    }
    // cari player yang property count nya = maxPropertyCount
    std::vector<Player*> propertyWinners;
    for (Player* player : balanceWinners) {
        if (player->getPropertyCount() == maxPropertyCount) {
            propertyWinners.push_back(player);
        }
    }
    // kalo cuma 1 yang properti nya paling banyak
    if (propertyWinners.size() == 1) {
        view_.showMessage("Pemenang: " + propertyWinners[0]->getUsername() + "\n");
        view_.showMessage("Uang: M" + std::to_string(propertyWinners[0]->getBalance()) + "\n");
        view_.showMessage("Jumlah properti: " + std::to_string(propertyWinners[0]->getPropertyCount()) + "\n");
        return;
    }

    // kartu terbanyak
    int maxCardCount = propertyWinners[0]->getCardCount();
    for (Player* player : propertyWinners) {
        if (player->getCardCount() > maxCardCount) {
            maxCardCount = player->getCardCount();
        }
    }
    // cari player yang card count nya = maxCardCount
    std::vector<Player*> finalWinners;
    for (Player* player : propertyWinners) {
        if (player->getCardCount() == maxCardCount) {
            finalWinners.push_back(player);
        }
    }

    // pemenang
    if (finalWinners.size() == 1) {
        view_.showMessage("Pemenang: " + finalWinners[0]->getUsername() + "\n");
        view_.showMessage("Uang: M" + std::to_string(finalWinners[0]->getBalance()) + "\n");
        view_.showMessage("Jumlah properti: " + std::to_string(finalWinners[0]->getPropertyCount()) + "\n");
        view_.showMessage("Jumlah kartu: " + std::to_string(finalWinners[0]->getCardCount()) + "\n");
    } else { // size != 1
        view_.showMessage("Permainan berakhir seri. Para pemenang:\n");
        for (Player* player : finalWinners) {
            view_.showMessage("- " + player->getUsername() +
                            " | Uang: M" + std::to_string(player->getBalance()) +
                            " | Properti: " + std::to_string(player->getPropertyCount()) +
                            " | Kartu: " + std::to_string(player->getCardCount()) +
                            "\n");
        }
    }
    // Pemenang adalah pemain dengan uang terbanyak.
    // Jika seri, pemain yang memiliki jumlah petak properti
        // terbanyak yang menjadi pemenang.
    // Jika seri, pemain yang memiliki jumlah
        // kartu terbanyak yang menjadi pemenang.
    // Jika seri, semua pemain yang masih
        //seri menjadi pemenang.
    // Jika kondisi bankruptcy terjadi sebelum
        // mencapai batas maksimum giliran,
        //ikuti aturan bankruptcy.
}

bool GameController::hasSoleWinner() const{
    size_t countNotBankrupt = 0;
    for (auto& player : players_) {
        if (!player->isBankrupt()) {
            countNotBankrupt++;
        }
    }
    return countNotBankrupt == players_.size() - 1;
}