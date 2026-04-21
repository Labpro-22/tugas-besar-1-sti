#include "core/Monopoly.hpp";

void Monopoly::setMaxTurn(int maxTurn) {
    maxTurn_ = maxTurn;
}

void Monopoly::setInitialBalance(int initialBalance) {
    initialBalance_ = initialBalance;
}

void Monopoly::playGame() {
    // kondisi blm ada yang menang juga
    Command* c;//tar fix
    Vieww* v;//tarfix

    GameController gameController(board_, dice_, v, c);
    // buat player
    
    int i = 0;
    while (i < maxTurn_ && !hasWinner()) {
        for (Player& player : players_) {
            if (!player.isBankrupt()) {
                gameController.processTurn(player);
            }
        }
        i++;
    }
}

bool Monopoly::hasWinner() {
    int countBankrupt = 0;
    for (Player& player : players_) {
        if (player.isBankrupt()) {
            countBankrupt++;
        }
    }
    return countBankrupt == players_.size() - 1;
}