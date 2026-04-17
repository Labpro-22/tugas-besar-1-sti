#pragma once
#include "Tile.hpp"
#include <vector>
class Board
{
    private:
        // perlu simpan size ga :/
        std::vector<Tile*> tiles_;
    public:
        Board();
        ~Board();
        void addTile(Tile* tile);
};


