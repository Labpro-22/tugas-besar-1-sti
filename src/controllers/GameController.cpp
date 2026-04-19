#include "controllers/GameController.hpp"

GameController::GameController(Board& board, IDice& dice, GameViewInterface& view, CommandInterface& command)
    : board_(board), dice_(dice),  view_(view), command_(command) {}

GameController::~GameController() = default;

void GameController::processTurn(Player& p) {
    // Cek lagi dijail atau engga

    // kalo ga lagi dijail jalanin ini dulu
    chooseSpecialCard(p);

    // kalo td d

}

void GameController::chooseSpecialCard(Player& p) {

}

void GameController::dropExtraSpecialCard(Player& p) {

}

