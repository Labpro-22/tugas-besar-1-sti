#pragma once
#include <iostream>
#include "../injector/injector.hpp"
class Tile : public ITile{
    protected:
        int tileID_;
        std::string letterCode_; // kode petak dari properti
        std::string tileName_;
        std::string colourBlock_;
        
    public:
        Tile(int tileID, std::string letterCode,
            std::string tileName, std::string colourBlock);
        virtual ~Tile();

        virtual void executeTile(Player& p) = 0;
};