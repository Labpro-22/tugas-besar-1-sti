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
        virtual void showMessage(std::string) = 0;

        virtual void setBoardContext(Board*) {}

        // Kasih tunjuk boardnya
        virtual void cetakPapan(Player& p, int currentTurn) {}

        // Kasih pesan ketika roll dice
        // Kasih pesan mendaratnya
        virtual void lemparDadu() {}

        // roll settingan
        virtual void aturDadu() {}

        virtual void cetakAkta(const std::string& kodePetak) {}

        // cetak properti
        virtual void cetakProperti(Player* player) {}

        // kasih lihat mo beli
        virtual bool beliProperti() { return false; }
        virtual void beliRailroad() {}; //otomatis
        virtual void beliUtility() {}; //otomatis

        // bayar sewa
        virtual void bayarSewa() {};

        virtual void bayarPajak() {};

        virtual void gadai() {};

        virtual void tebus() {};

        virtual void bangun() {};

        // TO DO:
        // lelang???
        // kebangkrutan???
        
        virtual ~GameViewInterface() = default;
};
