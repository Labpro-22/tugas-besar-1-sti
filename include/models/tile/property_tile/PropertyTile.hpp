#pragma once
#include "models/tile/Tile.hpp"
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
        int getPurchasePrice() const;
        int getLevel() const;
        void setLevel(int level);
        int getMortgageValue() const;
        std::string getColourBlock() const;// dummy

        std::string getOwnerUsername() const;
        void setOwnerUsername(const std::string& ownerUsername);

        PropertyStatus getPropertyStatus() const;
        void setPropertyStatus(PropertyStatus status);

        int getFestivalMultiplier() const;
        void setFestivalMultiplier(int multiplier);

        int getFestivalDuration() const;
        void setFestivalDuration(int duration);

        bool isOwnedByBank() const;
        bool isOwnedBy(const std::string& username) const;

        void resetFestivalEffect();
        void resetAfterBankruptcyToBank();
        std::string getOwnerUsername() const;
        std::string setOwnerUsernameToBank();

        int canDoubleFestival() const; // ga bs double kalo itunya msh = 3 atau ga multipliernya = 8

        int doubleTheMultiplier();

        int resetTurnTo3(); // jadikan max lagi

        bool alreadyMaxMultiplier()const;

        bool festivalActive();

        bool isMortgaged() const;

        bool isOwned() const; // ya own aja asumsinya si pemanggil emg org yg punya ini

        bool hasBuilding() const; // levelnya >= 1

        void setToMortgaged();

        int getSellingPrice() const;

        int sellAllBuildings() const;
};