enum class CommandType {
    CETAK_PAPAN,
    LEMPAR_DADU, //
    ATUR_DADU, // contoh : ATUR_DADU 2 5
    CETAK_AKTA, // bs petak manapun
    CETAK_PROPERTI, // lihat properti yg dipunya aja si
    GADAI, // bs properti manapun
    TEBUS, // properti manapun yang dimiliki
    BANGUN, // di manapun tapi s & k berlaku
    SIMPAN, // ke .txt mana gitu Hanya dapat dipanggil di awal giliran pemain (belum melakukan aksi apapun).
    MUAT, // dari state terakhir
    CETAK_LOG, //
    GUNAKAN_KEMAMPUAN, //
    BAYAR_DENDA, // tambahan buat lgsg bayar waktu mau keluar penjara


};

class Command {
    private:
        CommandType type_;
        std::vector<int> args_;
        std::string stringArg_;
    public:
        Command(CommandType type) : type_(type) {}
        Command(CommandType type, std::vector<int> args) :type_(type), args_(args) {}// mostly buat dice si
        Command(CommandType type, std::string stringArg) : type_(type), stringArg_(stringArg) {}

        CommandType getType() const {
            return type_;
        }

        int getArg(int idx) const {
            return args_.at(idx);
        }

        int getArgCount() const {
            return static_cast<int>(args_.size());
        }

        std::string getStringArg() const {
            return stringArg_;
        }

};
class CommandInterface {
    public:
        virtual ~CommandInterface() = default;
        
        virtual Command getCommand() = 0;

        // batas atas batas bawah inclusive
        virtual int getInt(int lowerBound, int upperBound) = 0;

        virtual int getAuctionBidOrPass(int currentBid, int playerBalance) = 0;

        // Game setup
        virtual std::string askFolderForConfig() = 0;
        virtual int askNumOfPlayer() = 0;
        virtual std::string askPlayerUsername() = 0;
        virtual bool askWantToLoadState() = 0;
        virtual std::string getTileToGetFestival() = 0;

        // buy
        virtual bool askWantToBuyProperty() = 0;
};