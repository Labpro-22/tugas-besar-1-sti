#pragma once
#include <iostream>
#include "../injector/injector.hpp"
#include "../board/Board.hpp"


// belom ada : urutan turn
class Player {
    public:
        enum PlayerStatus {
            ACTIVE,
            BANKRUPT,
            JAILED
        };
        // kasih turn kapan???
        Player(std::string username, int initialBalance, int playerTurn);
        ~Player();
        
        int getBalance() const;
        void addMoney(int amount);
        void deductMoney(int amount);
        
        PlayerStatus getStatus() const;
        void setStatus(PlayerStatus status);
        
        int getPosition() const;
        void setPosition(int pos);
        
        // Untuk Penjara
        bool thisTurnFreeFromJail() const;
        void incrementJailTurn();
        int getCountJail() const;
        void resetJailTurn();

        // Untuk PPH
        int getTotalPropertyValue() const;
        int getTotalBuildingValue() const;

        // cek bankrupt atau engga
        bool isBankrupt() const;
        

    private:
        int id_; // auto increment dari countplayer itu
        std::string username_; //username pemain
        int balance_; // jumlah uang pemain
        int position; // kode petak tempat pemain berada
        PlayerStatus status_; // default ACTIVE
        int doubleRollCount_; // default 0
        int countJail_; // default
        int playerTurn_; // urutan ke berapa
        // Inventory inventory_; // belom ada kelasnya wait ye
        static inline int countPlayer = 0;
};
