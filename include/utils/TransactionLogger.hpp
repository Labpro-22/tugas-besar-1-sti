#include <iostream>
#include <vector>
#include <string>
#include "Formatter.hpp"


// Note : plis banget cek lagi di spek 
// Buat di .cpp aja ya, tolong rapikan :) thanks!
class TransactionLogger {
    private:
        static inline std::vector<std::string> logs;
        static inline int turn; // minta si pusat yg nambahin aja
        static void addNewLog(std::string log);

    public:
        static void  incrementTurn();


        // no_turn uname_pemain jenis_aksi, detail_aksi

        // 1 Uname1 BELI Beli Jakarta (JKT) seharga M400
        static void logDiceRollAndMovement(std::string username, 
            int die1, int die2, std::string tileName, std::string tileCode) {
            std::string detail = "Lempar: " + std::to_string(die1) + "+" + std::to_string(die2) + "=" + std::to_string(die1 + die2) + "mendarat di " + Formatter::formatTileNameAndCode(tileName, tileCode);
            logs.push_back(Formatter::formatLog(turn, username, "DICE", detail));
        }

        // 1 Uname2 DADU Lempar: 4+5=9 mendarat di Surabaya (SBY)
        static void logBuyProperty(std::string username, std::string tileName, std::string tileCode, int money) {
            std::string detail = "Beli " + Formatter::formatTileNameAndCode(tileName, tileCode) + " seharga " + Formatter::formattingMoney(money);
            logs.push_back(Formatter::formatLog(turn, username, "BELI", detail));
        }

        // Bayar_sewa atau gmn bebas deh nulisnya gmn
        // 1 PEMBAYAR BAYAR_SEWA Bayar sewa di Jakarta (JKT) seharga M400 kepada PENERIMADUITT
        static void logPayRent(std::string);

        // 2 PEMBAYAR BAYAR_PAJAK Bayar pajak PPH sebesar M500
        static void logPayTax();

        // 2 PEMBANGUN BELI_BANGUNAN Beli bangunan di Bandung (BDG) sehingga meningkat dari level X ke level Y seharga M900
        static void logBuyBuilding();

        // 2 PEMILIK JUAL_BANGUNAN Jual bangunan di Bandung (BDG) sehingga turun dari level Y ke level X memperoleh M1000
        static void logSellBuilding();

        // 3 PEMILIK TEBUS Tebus property Bandung(BDG) seharga M700
        static void logRedeemProperty();

        // 4 PEMILIK GADAI Gadai property Bandung(BDG) memperoleh M300
        static void logMortgageProperty();

        // 3 PEMILIK SPECIAL_CARD menggunakan special card Demolition Card sehingga Efeknya-apa ke game
        static void logSpecialCardUse();

        // 4 PEMILIK CHANCE_CARD menggunakan chance card NAMANYA APA sehingga EFEKNYA APA
        static void logChanceCard();

        // 3 PEMILIK GENERAL_FUND menggunakan general fund card NAMANYA APA sehingga EFEKNYA APA
        static void logGeneralFund();

        // 4 PENAWAR TAWAR Tawar Bandung (Bdg) sebesar M700
        static void logAuctionBid();

        // 4 HASIL LELANG ...
        static void logAuctionFinalResult();

        // 4 YGDAPAT AMBIL_ALIH ambil alih aset Bandung (BDG) dari pemilik-semula
        static void logAssetTakeover();

        // 4 USN bankrut karena .....
        static void logBankruptCondition();

        //atur" la
        // save game ke mana 
        static void logSaveGame();

        // load game ke ...
        static void logLoadGame();
};