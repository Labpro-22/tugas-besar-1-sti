#pragma once
#include "Tile.hpp"
#include <map>
enum PropertyStatus {
    BANK,
    OWNED,
    MORTGAGED
};

class PropertyTile : public Tile {
    protected:
        std::string ownerUsername_; // berisi username pemilik atau BANK jika belum dimiliki
        int festivalMultiplier_; // harga bangunan akibat festival yang bernilai 1, 2, 4, atau 8 (1 = tidak aktif)
        int festivalDuration_; // adalah durasi dari festival yang bernilai 0–3 (0 = tidak aktif)
        PropertyStatus propertyStatus_; //  status dari properti yang bernilai BANK, OWNED, atau MORTGAGED
        int level_; // bernilai 0–4 untuk rumah, atau H untuk hotel. Untuk properti selain street, isi dengan 0.
                    // conflict dengan "kosong (Level 0) hingga hotel (Level 5). "
                    
        int mortgageValue_; // nilai gadai
        int purchasePrice_; // harga beli pertama kali

    public:
        PropertyTile(int tileID, std::string letterCode,
                    std::string tileName, std::string colourBlock,
                    // tambahan di PropertyTile
                    int purchasePrice, int mortgageValue);
        virtual ~PropertyTile();
};