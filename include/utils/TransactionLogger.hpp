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
        static void logDiceRollAndMovement(std::string username, std::string tileName, std::string tileCode, int money) {
            std::string detail = "Beli " + Formatter::formatTileNameAndCode(tileName, tileCode) + " seharga " + Formatter::formattingMoney(money);
            logs.push_back(Formatter::formatLog(turn, username, "BELI", detail));
        }
            
        // 1 Uname2 DADU Lempar: 4+5=9 mendarat di Surabaya (SBY)
        static void logBuyProperty(std::string username, int die1, int die2, std::string tileName, std::string tileCode) {
            std::string detail = "Lempar: " + std::to_string(die1) + "+" + std::to_string(die2) + "=" + std::to_string(die1 + die2) + "mendarat di " + Formatter::formatTileNameAndCode(tileName, tileCode);
            logs.push_back(Formatter::formatLog(turn, username, "DICE", detail));
        }

        // Bayar_sewa atau gmn bebas deh nulisnya gmn
        // 1 PEMBAYAR BAYAR_SEWA Bayar sewa di Jakarta (JKT) seharga M400 kepada PENERIMADUITT
        static void logPayRent(std::string username, std::string tileName, std::string tileCode, int money, std::string penerima) {
            std::string detail = "Bayar sewa di " + Formatter::formatTileNameAndCode(tileName, tileCode) + " seharga " + Formatter::formattingMoney(money) + " kepada " + penerima;
            logs.push_back(Formatter::formatLog(turn, username, "BAYAR_SEWA", detail));
        }

        // 2 PEMBAYAR BAYAR_PAJAK Bayar pajak PPH sebesar M500
        static void logPayTax(std::string username, std::string tileName, std::string tileCode, int money) {
            std::string detail = "Bayar pajak " + tileCode + " sebesar " + Formatter::formattingMoney(money);
            logs.push_back(Formatter::formatLog(turn, username, "BAYAR_PAJAK", detail));
        }

        // 2 PEMBANGUN BELI_BANGUNAN Beli bangunan di Bandung (BDG) sehingga meningkat dari level X ke level Y seharga M900
        static void logBuyBuilding(std::string username, std::string tileName, std::string tileCode, int oldLevel, int newLevel, int money) {
            std::string detail = "Beli bangunan di  " + Formatter::formatTileNameAndCode(tileName, tileCode) + " sehingga meningkat dari level " + std::to_string(oldLevel) + " ke level " + std::to_string(newLevel) + " seharga " + Formatter::formattingMoney(money);
            logs.push_back(Formatter::formatLog(turn, username, "BELI_BANGUNAN", detail));
        }

        // 2 PEMILIK JUAL_BANGUNAN Jual bangunan di Bandung (BDG) sehingga turun dari level Y ke level X memperoleh M1000
        void TransactionLogger::logSellBuilding(std::string username, std::string tileName, std::string tileCode, int oldLevel, int newLevel, int money) {
            std::string detail = "Jual bangunan di " + Formatter::formatTileNameAndCode(tileName, tileCode) + " sehingga turun dari level " + std::to_string(oldLevel) + " ke level " + std::to_string(newLevel) + " memperoleh " + Formatter::formattingMoney(money);
            addNewLog(Formatter::formatLog(turn, username, "JUAL_BANGUNAN", detail));
        }

        // 3 PEMILIK TEBUS Tebus property Bandung (BDG) seharga M700
        void TransactionLogger::logRedeemProperty(std::string username, std::string tileName, std::string tileCode, int money) {
            std::string detail = "Tebus property " + Formatter::formatTileNameAndCode(tileName, tileCode) + " seharga " + Formatter::formattingMoney(money);
            addNewLog(Formatter::formatLog(turn, username, "TEBUS", detail));
        }

        // 4 PEMILIK GADAI Gadai property Bandung (BDG) memperoleh M300
        void TransactionLogger::logMortgageProperty(std::string username, std::string tileName, std::string tileCode, int money) {
            std::string detail = "Gadai property " + Formatter::formatTileNameAndCode(tileName, tileCode) + " memperoleh " + Formatter::formattingMoney(money);
            addNewLog(Formatter::formatLog(turn, username, "GADAI", detail));
        }

        // 3 PEMILIK SPECIAL_CARD Menggunakan special card Demolition Card sehingga ...
        void TransactionLogger::logSpecialCardUse(std::string username, std::string cardName, std::string effect) {
            std::string detail = "Menggunakan special card " + cardName + " sehingga " + effect;
            addNewLog(Formatter::formatLog(turn, username, "SPECIAL_CARD", detail));
        }

        // 4 PEMILIK CHANCE_CARD menggunakan chance card NAMA sehingga EFEK
        void TransactionLogger::logChanceCard(std::string username, std::string cardName, std::string effect) {
            std::string detail = "Menggunakan chance card " + cardName + " sehingga " + effect;
            addNewLog(Formatter::formatLog(turn, username, "CHANCE_CARD", detail));
        }

        // 3 PEMILIK COMMUNITY_CHEST menggunakan general fund card NAMANYA APA sehingga EFEKNYA APA
        void TransactionLogger::logCommunityChest(std::string username, std::string cardName, std::string effect) {
            std::string detail = "Menggunakan community chest card " + cardName + " sehingga " + effect;
            addNewLog(Formatter::formatLog(turn, username, "COMMUNITY_CHEST", detail));
        }

        // 4 PENAWAR TAWAR Tawar Bandung (BDG) sebesar M700
        void TransactionLogger::logAuctionBid(std::string username, std::string tileName, std::string tileCode, int money) {
            std::string detail = "Tawar " + Formatter::formatTileNameAndCode(tileName, tileCode) + " sebesar " + Formatter::formattingMoney(money);
            addNewLog(Formatter::formatLog(turn, username, "TAWAR", detail));
        }

        // 4 YGDAPAT HASIL_LELANG Menang lelang Bandung (BDG) seharga M700
        void TransactionLogger::logAuctionFinalResult(std::string winnerName, std::string tileName, std::string tileCode, int money) {
            std::string detail = "Menang lelang " + Formatter::formatTileNameAndCode(tileName, tileCode) + " seharga " + Formatter::formattingMoney(money);
            addNewLog(Formatter::formatLog(turn, winnerName, "HASIL_LELANG", detail));
        }

        // 4 YGDAPAT AMBIL_ALIH Ambil alih aset Bandung (BDG) dari pemilik-semula
        void TransactionLogger::logAssetTakeover(std::string newOwner, std::string tileName, std::string tileCode, std::string oldOwner) {
            std::string detail = "Ambil alih aset " + Formatter::formatTileNameAndCode(tileName, tileCode) + " dari " + oldOwner;
            addNewLog(Formatter::formatLog(turn, newOwner, "AMBIL_ALIH", detail));
        }

        // 4 USN BANGKRUT Bangkrut karena .....
        void TransactionLogger::logBankruptCondition(std::string username, std::string reason) {
            std::string detail = "Bangkrut karena " + reason;
            addNewLog(Formatter::formatLog(turn, username, "BANGKRUT", detail));
        }

        // SAVE_GAME dan LOAD_GAME biasanya tidak punya 'username' yang spesifik, kita bisa pakai "SISTEM"
        void TransactionLogger::logSaveGame(std::string filename) {
            std::string detail = "Menyimpan permainan ke file " + filename;
            addNewLog(Formatter::formatLog(turn, "SISTEM", "SAVE_GAME", detail));
        }

        void TransactionLogger::logLoadGame(std::string filename) {
            std::string detail = "Memuat permainan dari file " + filename;
            addNewLog(Formatter::formatLog(turn, "SISTEM", "LOAD_GAME", detail));
        }
};