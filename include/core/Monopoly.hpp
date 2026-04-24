#pragma once
#include "models/player/Player.hpp"
#include "models/board/Board.hpp"
#include "controllers/GameController.hpp"
#include "models/dice/Dice.hpp"
#include <iostream>
#include <memory>
#include "views/GameViewInterface.hpp"
#include <algorithm>
#include <random>
#include "models/card/skillcard/SkillCard.hpp"

class Monopoly {
    private:
        std::unique_ptr<GameViewInterface> view_;
        std::unique_ptr<CommandInterface> command_;
        static inline int maxTurn_;
        static inline int initialBalance_;

        std::vector<std::unique_ptr<Player>> registerPlayers(int numOfPlayers);
        void shufflePlayersTurn(std::vector<std::unique_ptr<Player>>& players_);
        std::unique_ptr<Board> loadConfig();
        
        public:
        bool loadState(std::unique_ptr<Board>& board, std::vector<std::unique_ptr<Player>>& players, int& latestTurn);
        Monopoly(std::unique_ptr<GameViewInterface> view, std::unique_ptr<CommandInterface> command);
        ~Monopoly() = default;

        static void setMaxTurn(int maxTurn);
        static void setInitialBalance(int initialBalance);

        void startGame();
};
