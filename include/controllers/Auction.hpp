#pragma once

#include <memory>
#include <string>
#include <vector>

#include "models/player/Player.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include "views/CommandInterface.hpp"
#include "views/GameViewInterface.hpp"

class Auction {
public:
    // penyebab lelang
    enum class AuctionCause {
        PROPERTY_DECLINED,
        PROPERTY_CANNOT_AFFORD,
        BANKRUPTCY_TO_BANK
    };

    enum class AuctionState {
        NOT_STARTED,
        WAITING_FOR_ACTION,
        FINISHED
    };

private:
    std::vector<std::unique_ptr<Player>>& players_; // daftar player yang main
    GameViewInterface& view_; // output 

    PropertyTile* property_; // property yang di lelang
    Player* triggerPlayer_; // pemain yang memicu lelang
    AuctionCause cause_; // penyebab lelang
    AuctionState state_; // state lelang

    std::vector<Player*> participants_; // urutan pemain yang ikut lelang
    int currentParticipantIndex_; // index di participants_ untuk giliran sekarang

    int highestBid_; // penawaran tertinggi 
    Player* highestBidder_; // pemain dengan penawaran tertinggi
    int passesCount_; // jumlah pass beruntun untuk mendeteksi forced bid
    bool hasAnyBid_; // apakah sudah ada penawaran
    bool finished_; // apakah lelang sudah selesai

    void buildParticipants(); // bikin daftar peserta yang valid untuk lelang
    bool isValidParticipant(const Player* player) const; // cek pemain no BANKRUPT
    int findTriggerPlayerIndex() const; // cari index triggerPlayer di players_

    bool isForcedBidTurn() const; // cek pemain boleh pass ga? karena sebelumnya sudah pass semua
    void advanceTurn(); // lanjut ke peserta berikutnya
    void finalizeAuction(); // proses akhir lelang, transfer properti ke pemenang, update state, dan tampilkan hasil
    void givePropertyToWinner(); // transfer properti ke pemenang dan update uangnya

public:
    Auction(std::vector<std::unique_ptr<Player>>& players,
            GameViewInterface& view);

    ~Auction();

    // Session setup
    void start(Player& triggerPlayer, PropertyTile& propertyTile, AuctionCause cause); // mulai lelang 

    bool canCurrentPlayerPass() const; // cek apakah pemain saat ini boleh pass 
    bool canCurrentPlayerBid(int amount) const; // cek apakah pemain saat ini boleh bid 

    void passCurrentPlayer(); // pemain saat ini memilih pass 
    void bidCurrentPlayer(int amount); // pemain saat ini melakukan bid dengan jumlah tertentu

    // proses lelang ketika pemain bangkrut ke bank
    void processBankruptcyToBank(Player& bankruptPlayer);

    bool isStarted() const; // bantu GUI cek apakah lelang sedang berlangsung
    bool isFinished() const; // bantu GUI cek apakah lelang sudah selesai

    AuctionState getState() const; 
    AuctionCause getCause() const;

    PropertyTile* getProperty() const; // properti yang sedang dilelang
    Player* getTriggerPlayer() const; // pemain yang memicu lelang
    Player* getCurrentPlayer() const; // pemain yang sedang giliran lelang
    Player* getHighestBidder() const; // pemain dengan penawaran tertinggi

    int getHighestBid() const; // jumlah penawaran tertinggi saat ini
    int getCurrentParticipantIndex() const; // index peserta yang sedang giliran lelang
    int getpassesCount() const; // jumlah pass beruntun saat ini, untuk mendeteksi apakah pemain berikutnya wajib bid
    int getParticipantSize() const; // jumlah peserta lelang saat ini

    int getMinBid() const; // dapatkan jumlah minimum untuk bid berikutnya (1 lebih tinggi dari highestBid, atau 0 jika belum ada bid)

    const std::vector<Player*>& getParticipants() const; // daftar peserta lelang saat ini, untuk GUI tampilkan daftar pemain yang ikut

    std::string getStatusText() const; // untuk GUI tampilkan status lelang, termasuk properti yang dilelang, penyebab lelang, pemain dengan penawaran tertinggi, dan giliran pemain saat ini

    void reset(); // reset state internal lelang, untuk dipakai sebelum start() agar bisa dipakai berulang kali untuk properti berikutnya

    void runAuction(Player& triggerPlayer, PropertyTile& propertyTile, AuctionCause cause, CommandInterface& command);

    void runBankruptcyAuction(Player& bankruptPlayer, CommandInterface& command);
};

/*
    Flow untuk Auction :    
    1. Auction object dibuat
    2. Ketika trigger event terjadi, panggil auction.start(triggerPlayer, propertyTile, cause)
       - auction akan otomatis membangun daftar peserta yang valid berdasarkan triggerPlayer dan cause
       - jika tidak ada peserta valid, lelang langsung selesai dengan properti tetap di BANK
       - jika ada peserta, lelang dimulai dengan urutan peserta setelah triggerPlayer
    3. Selama auction belum selesai, pemain dapat melakukan aksi pass atau bid melalui auction.passCurrentPlayer() dan auction.bidCurrentPlayer(amount)
    4. Auction update state internal untuk melacak penawaran tertinggi, jumlah pass, dan menentukan kapan lelang selesai
    5. Ketika lelang selesai, auction.finalizeAuction() akan dipanggil untuk memberikan properti kepada pemenang (jika ada) dan melakukan update yang diperlukan
    6. Proses lelang untuk properti berikutnya (jika ada) dapat dipicu lagi dengan memanggil auction.start(...) dari luar
*/