#pragma once


// pertanyaan : minta input reader dllnya gimana ya?? sama kapan gitu
#include "../models/player/Player.hpp"
#include "../models/board/Board.hpp"
class Monopoly {
    private:
        static inline int maxTurn_;
        static inline int initialBalance_;
        
        // msh ragu make * sm & apa gmn 
        std::vector<Player*> players_;
        Board& board_; 

    public:
        Monopoly();
        ~Monopoly();

        static void setMaxTurn(int maxTurn);
        static void setInitialBalance(int initialBalance);
        
        // selesai ketika ada yg menang atau udh maxturn
        void playGame();
        // 1 giliran artinya jalanin semua player (kalo ga bankrut)
        void playTurn();
};
