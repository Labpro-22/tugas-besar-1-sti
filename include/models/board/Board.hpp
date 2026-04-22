#pragma once
#include "models/tile/Tile.hpp"
#include <vector>
class Board
{
    private:
        // perlu simpan size ga :/
        int size_; // default
        int jailPosition_;
        int startPosition_;
        std::vector<Tile&> tiles_;
    public:
        Board(int size);
        ~Board();
        void addTile(Tile& tile);
        
        // geser ke tile setelah ditambahin distance (hasil roll dadu)
        Tile& moveToNextTile(int distance);
        Tile& getCurrentTile(int id);
        bool has(std::string code);
        

        int getJailPosition() const;
        void setJailPosition(int pos);
        int getStartPosition() const;
        void setStartPosition(int pos);

};


