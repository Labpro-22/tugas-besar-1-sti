#include "models/tile/Tile.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/tile/property_tile/UtilityTile.hpp"
#include "models/player/Player.hpp"
#include "models/tile/action_tile/tax_tile/TaxTile.hpp"
class GameViewInterface {
    public:
        // Kasih tunjuk boardnya
        virtual void cetakPapan();

        // Kasih pesan ketika roll dice
        // Kasih pesan mendaratnya
        virtual void lemparDadu();

        // roll settingan
        virtual void aturDadu();

        virtual void cetakAkta(const Tile& tile);

        // cetak properti
        virtual void cetakProperti(const PropertyTile& propertyTile);

        // kasih lihat mo beli
        virtual bool beliProperti(const Player& player, const PropertyTile& propertyTile);
        virtual void beliRailroad(const RailRoadTile& railRoadTile); //otomatis
        virtual void beliUtility(const UtilityTile& utilityTile); //otomatis

        // bayar sewa
        virtual void bayarSewa(const Player& player, const PropertyTile& propertyTile);

        virtual void bayarPajak(const Player& player, const TaxTile& taxTile);

        virtual void gadai(const Player& player);

        virtual void tebus(const Player& player);

        virtual void bangun(const Player& player);


        // lelang???
        // kebangkrutan???
        
        virtual ~GameViewInterface() = default;
};
