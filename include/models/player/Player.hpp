#pragma once
#include <iostream>
// #include "models/inventory/Inventory.hpp"
#include "../injector/injector.hpp"
#include "../board/Board.hpp"

// class Inventory;
enum PlayerStatus {
    ACTIVE,
    BANKRUPT,
    JAILED
};

// belom ada : urutan turn
class Player {
    private:
        int id_; // auto increment dari countplayer itu
        std::string username_; //username pemain
        int balance_; // jumlah uang pemain
        int position; // kode petak tempat pemain berada
        PlayerStatus status_; // default ACTIVE
        int doubleRollCount_; // default 0
        int countJail_; // default
        int playerTurn; // urutan ke berapa
        // Inventory inventory_; // belom ada kelasnya wait ye
        static inline int countPlayer = 0;

        // Dice
        IDice* dice_;

        // Board
        Board& board_;
    public:
        // kasih turn kapan???
        Player(std::string username, Board* board, IDice* dice);
        ~Player();

        // full 1 turn dia jalan roll dice dll-nya
        void takeTurn();
        
        int getBalance() const;
        void addMoney(int amount);
        void deductMoney(int amount);
        
        PlayerStatus getStatus() const;
        void setStatus(PlayerStatus status);
        
        int getPosition() const;
        void setPosition(int pos);
        
        Board& getBoard() const;
        
        // Untuk Penjara
        void incrementJailTurn();
        int getCountJail() const;
        void resetJailTurn();

        // Untuk PPH
        int getTotalPropertyValue() const;
        int getTotalBuildingValue() const;
        
        // Untuk referensi ke Monopoly (sesuai diagram terakhir)
        class Monopoly& getMonopoly();
};
