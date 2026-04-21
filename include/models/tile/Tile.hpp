#pragma once
#include <iostream>
#include "../injector/injector.hpp"

// Forward Declaration, karena ada looping include: Tile -> Player -> Board -> Tile. Mencegah itu dengan ini, kalau error tinggal uncomment saja
// class Player;

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

        virtual void onLand(Player& p) = 0;
        virtual void getDetail() const;

        // Getter
        int getTileID() const;
        std::string getLetterCode() const;
        std::string getTileName() const;
        std::string getColourBlock() const;
};