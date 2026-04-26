#pragma once
#include "models/tile/Tile.hpp"
#include <memory>
#include <vector>
#include <utility>
#include <map>
#include "models/exception/InvariantViolationException/InvariantViolationException.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
class Board
{
    private:
        // perlu simpan size ga :/
        int size_; // default
        int jailPosition_;
        int startPosition_;
        std::vector<std::unique_ptr<Tile>> tiles_;
        std::map<std::string, size_t> countTilesForEachColourBlock_;
    public:
        Board(int size);
        ~Board();
        Board(Board&&) = default;
        Board& operator=(Board&&) = default;
        void addTile(std::unique_ptr<Tile> tile);
        
        // geser ke tile setelah ditambahin distance (hasil roll dadu)
        Tile& moveToNextTile(int distance);
        Tile& getCurrentTile(int id);
        Tile& getTileByCode(const std::string& code);
        int getTileIndexByCode(const std::string& code) const;
        int getSize() const;
        bool has(std::string code);


        bool isCompletedColourGroup(std::string ownerName, std::string colourBlock);
        
        int getJailPosition() const;
        void setJailPosition(int pos);
        int getStartPosition() const;
        void setStartPosition(int pos);
        std::map<std::string, size_t> getCountTilesForEachColourBlock() const;
        void addCountTilesForEachColourBlock();
};


