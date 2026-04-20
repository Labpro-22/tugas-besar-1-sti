#pragma once
#include "models/tile/Tile.hpp"
#include <vector>
class Board
{
    private:
        // perlu simpan size ga :/
        std::vector<Tile&> tiles_;
    public:
        Board();
        ~Board();
        void addTile(Tile& tile);
        
        // geser ke tile setelah ditambahin distance (hasil roll dadu)
        Tile& moveToNextTile(int distance);
        Tile& getCurrentTile(int id);

        int getJailPosition();
};


