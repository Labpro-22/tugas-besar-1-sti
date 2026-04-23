#pragma once
#include <iostream>
#include "../injector/injector.hpp"
#include "models/board/Board.hpp"
#include "models/inventory/Inventory.hpp"
#include "models/card/skillcard/SkillCard.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include <functional>
#include <memory>
#include <cstddef>
#include <algorithm>
#include <vector>
// belom ada : urutan turn
class Player {
    public:
        enum PlayerStatus {
            ACTIVE,
            BANKRUPT,
            JAILED
        };
        // kasih turn kapan???
        Player(std::string username, int initialBalance);
        ~Player();

        // GETTER USERNAME
        std::string getUsername() const;

        // SET TURN
        void setTurn(int turn);

        // MONEY RELATED
        int getBalance() const;
        void addMoney(int amount);
        void deductMoney(int amount);

        // STATUS RELATED
        PlayerStatus getStatus() const;
        void setStatus(PlayerStatus status);

        // MOVEMENT OR POSITION RELATED
        int notViolatingDoubleRollCount() const;
        int getPosition() const;
        void setPosition(int pos);
        int move(int displacement);
        void incrementDoubleCount();
        bool safeToGetMoreDouble() const;
        
        // JAIL RELATED
        bool isInJail() const;
        bool thisTurnAutoFreeFromJail() const;
        void incrementJailTurn();
        int getCountJail() const;
        void resetJailTurn();

        // Untuk PPH
        int getTotalPropertyValue() const;
        int getTotalBuildingValue() const;

        // cek bankrupt atau engga
        bool isBankrupt() const;


        // Inventory related
        std::vector<SkillCard*> getSkillCards();
        std::vector<PropertyTile*> getProperties();

        void addProperty(PropertyTile* propertyTile);
        void removeProperty(PropertyTile* propertyTile);
        bool ownsProperty(PropertyTile* propertyTile) const;

        Inventory& getInventory();

        int getLastDiceTotal() const;
        void setLastDiceTotal(int diceTotal);
        bool hasProperty(std::string tileCode);
        PropertyTile& getProperty(std::string code);
        // shield
        void activateShield(int turns = 1);
        bool isShielded() const;
        void consumeShield();

        // discount
        void activateDiscount(int percent, int turns = 1);
        bool isDiscounted() const;
        int getDiscountPercent() const;
        void consumeDiscount();

        // Inventory related
        void addSkillCard(std::unique_ptr<SkillCard> skillCard);
        std::size_t getSkillCardCount() const;
        const SkillCard* getSkillCardAt(std::size_t idx) const;
        std::unique_ptr<SkillCard> takeSkillCard(std::size_t idx);

        static const std::vector<Player*>& getAllPlayers();
        

    private:
        int id_; // auto increment dari countplayer itu
        std::string username_; //username pemain
        int balance_; // jumlah uang pemain
        int position_; // kode petak tempat pemain berada
        PlayerStatus status_; // default ACTIVE
        int doubleRollCount_; // default 0
        int countJail_; // default
        int playerTurn_; // urutan ke berapa
        int shieldTurns_; // shield turn
        int discountPercent_; // discount percent
        int discountTurns_; // discount turn
        Inventory inventory_; // belom ada kelasnya wait ye
        static inline int countPlayer = 0;
        int lastDiceTotal_;
        static inline std::vector<Player*> allPlayers_ = {};
};
