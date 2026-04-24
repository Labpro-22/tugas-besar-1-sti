#pragma once
#include <iostream>
#include "../injector/injector.hpp"
#include "views/CommandInterface.hpp"
#include "views/GameViewInterface.hpp"

enum class OnLandResult {
    Done,
    TriggerAuction,
    TakeChanceCard,
    TakeCommunityChest,
    TriggerBankruptcyAuction, //
    Festival,
    TriggerMoveToJail,
    TriggerBankruptcyFlow,
    TriggerCanPayRentUsingCash,
    TriggerTryToPayRent
    // tambahin lagi
    
};

class Tile {
    protected:
        int tileID_;
        std::string letterCode_; // kode petak dari properti
        std::string tileName_;
        std::string colourBlock_;
        
    public:
        // Ctro & Dtor
        Tile(int tileID, std::string letterCode,
            std::string tileName, std::string colourBlock);
        virtual ~Tile();

        virtual OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) = 0;
        virtual void getDetail() const;

        // Getter
        int getTileID() const;
        std::string getLetterCode() const;
        std::string getTileName() const;
        std::string getColourBlock() const;
};