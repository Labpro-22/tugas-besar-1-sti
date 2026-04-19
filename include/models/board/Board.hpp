#pragma once
#include "../injector/injector.hpp"
#include <vector>
class Board
{
    private:
        // perlu simpan size ga :/
        std::vector<ITile*> tiles_;
    public:
        Board();
        ~Board();
        void addTile(ITile* tile);
        
        // geser ke tile setelah ditambahin distance (hasil roll dadu)
        ITile* moveToNextTile(int distance);

        int getJailPosition();
};


