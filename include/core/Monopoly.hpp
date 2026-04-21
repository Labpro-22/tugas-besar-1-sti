#pragma once
#include "models/player/Player.hpp"
#include "models/board/Board.hpp"
#include "controllers/GameController.hpp"
#include "models/dice/Dice.hpp"
#include <iostream>

class Command; // either CLI / gui
class Vieww; // either CLI/ gui
class Monopoly {
    private:
        static inline int maxTurn_;
        static inline int initialBalance_;
        
        std::vector<Player&> players_;
        Board& board_;
        Dice& dice_;

    public:
        Monopoly();
        ~Monopoly();

        static void setMaxTurn(int maxTurn);
        static void setInitialBalance(int initialBalance);
        
        // selesai ketika ada yg menang atau udh maxturn
        void playGame();

        bool hasWinner();
};
