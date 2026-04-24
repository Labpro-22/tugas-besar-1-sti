#pragma once
#include "models/tile/Tile.hpp"
#include <memory>
#include <vector>
#include <utility>
#include <map>

class Board
{
    private:
        // perlu simpan size ga :/
        int size_; // default
        int jailPosition_;
        int startPosition_;
        std::vector<std::unique_ptr<Tile>> tiles_;
        std::map<std::string, int> countTilesForEachColourBlock_;
    public:
        Board(int size);
        ~Board();
        void addTile(std::unique_ptr<Tile> tile);
        
        // geser ke tile setelah ditambahin distance (hasil roll dadu)
        Tile& moveToNextTile(int distance);
        Tile& getCurrentTile(int id);
        bool has(std::string code);


        bool isCompletedColourGroup(std::string ownerName, std::string colourBlock);
        int countOwnedUtilityTile(std::string username);
        int countOwnedRailRoadTile(std::string username);
        

        int getJailPosition() const;
        void setJailPosition(int pos);
        int getStartPosition() const;
        void setStartPosition(int pos);
        std::map<std::string, int> getCountTilesForEachColourBlock() const;
        void addCountTilesForEachColourBlock();

};


