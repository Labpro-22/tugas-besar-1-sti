#pragma once
#include <iostream>
#include "../inventory/Inventory.hpp"
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
        Inventory inventory_; // belom ada kelasnya wait ye
        static inline int countPlayer = 0;

    public:
        // kasih turn kapan???
        Player(std::string username);
        ~Player();

        // full 1 turn dia jalan roll dice dll-nya
        void takeTurn();
};
