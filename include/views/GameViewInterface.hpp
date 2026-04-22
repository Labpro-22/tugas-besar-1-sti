#include "models/tile/Tile.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/tile/property_tile/UtilityTile.hpp"
#include "models/player/Player.hpp"
#include "models/tile/action_tile/tax_tile/TaxTile.hpp"
class GameViewInterface {
    public:
        // Joker of all trades oakwoawkaw
        virtual void showMessage(std::string) = 0;

        // Kasih tunjuk boardnya
        virtual void cetakPapan();

        // Kasih pesan ketika roll dice
        // Kasih pesan mendaratnya
        virtual void lemparDadu();

        // roll settingan
        virtual void aturDadu();

        virtual void cetakAkta();

        // cetak properti
        virtual void cetakProperti();

        // kasih lihat mo beli
        virtual bool beliProperti();
        virtual void beliRailroad(); //otomatis
        virtual void beliUtility(); //otomatis

        // bayar sewa
        virtual void bayarSewa();

        virtual void bayarPajak();

        virtual void gadai();

        virtual void tebus();

        virtual void bangun();


        // lelang???
        // kebangkrutan???
        
        virtual ~GameViewInterface() = default;
};
