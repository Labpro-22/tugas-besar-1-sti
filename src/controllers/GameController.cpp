#include "controllers/GameController.hpp"

GameController::GameController(Board& board, IDice& dice, GameViewInterface& view, CommandInterface& command)
    : board_(board), dice_(dice),  view_(view), command_(command) {}

GameController::~GameController() = default;

void GameController::processTurn(Player& p) {
    Tile& currentTile = board_.getCurrentTile(p.getPosition());

    view_.kasihmsg("Turn player saat ini dimulai!\n");

    int displacement = 0;
    bool hasNotRollDice = true;

    // Kasus lagi di-jail
    if (p.isInJail()) {
        // harus coba roll dice dulu
        PrisonTile* prison = dynamic_cast<PrisonTile*>(&currentTile);
        if (p.thisTurnAutoFreeFromJail()) {
            view_.kasihmsg("Selamat! Player otomatis bebas dari penjara saat ini dengan membayar X duit!\n");
            p.deductMoney(prison->getFineCost());
            hasNotRollDice = false;
            
        } else {
            view_.kasihmsg("Kamu saat ini berada di penjara!\n Kamu hanya dapat melakukan rolldice\n");
            if (command_.RollSettingan()) {
                int die1 = command_.GetInt(); // angka 1
                int die2 = command_.GetInt(); // angka 2
                displacement = dice_.rollSettingan(die1, die2);
            } else {
                displacement = dice_.roll();
            }

            if (dice_.isDouble()) {
                // baru bs keluar
                Tile& nextTile = board_.moveToNextTile(displacement);
                nextTile.onLand(p);
                return;
            }
            p.incrementJailTurn();
        }
    }
    if (hasNotRollDice) {
        view_.kasihmsg("Saatnya memilih kartu!\n");
        // kartu dulu
        chooseSpecialCard(p);
        
        // roll dice
        if (command_.RollSettingan()) {
            int die1 = command_.GetInt(); // angka 1
            int die2 = command_.GetInt(); // angka 2
            displacement = dice_.rollSettingan(die1, die2);
        } else {
            displacement = dice_.roll();
        }
    }

    normalTurn(p, displacement);

}

void GameController::normalTurn(Player& p, int firstDisplacement) {
    Tile& nextTile = board_.moveToNextTile(firstDisplacement);
    nextTile.onLand(p);
    if (dice_.isDouble()) {
        command_.cmd("Selamat anda mendapatkan double! Silahkan lanjutkan giliran anda!\n");
        p.incrementDoubleCount();
    }

    while (dice_.isDouble() && p.safeToGetMoreDouble()) {
        int displacement = 0;

        // roll dice lagi
        if (command_.RollSettingan()) {
            int die1 = command_.GetInt(); // angka 1
            int die2 = command_.GetInt(); // angka 2
            displacement = dice_.rollSettingan(die1, die2);
        } else {
            displacement = dice_.roll();
        }

        nextTile = board_.moveToNextTile(displacement);
        nextTile.onLand(p);

        // add double count
        p.incrementDoubleCount();
    }
}

void GameController::chooseSpecialCard(Player& p) {

}

void GameController::dropExtraSpecialCard(Player& p) {

}

