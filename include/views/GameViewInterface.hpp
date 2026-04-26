#pragma once

#include <string>

class Tile;
class PropertyTile;
class RailRoadTile;
class UtilityTile;
class Player;
class TaxTile;
class Board;

// Ini tu jadi ViewManager (yang aku assume)
class GameViewInterface {
    protected: 

    public:
        // Joker of all trades oakwoawkaw
        virtual void showMessage(const std::string message) = 0;
        virtual void setBoardContext(Board* board) = 0;
        virtual void cetakPapan(Player& pl, int currentTurn) = 0;
        virtual void cetakAkta(const std::string& kodePetak) = 0;
        virtual void cetakProperti(Player* player) = 0;

        virtual ~GameViewInterface() = default;
};
