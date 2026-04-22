#include "controllers/GameController.hpp"

GameController::GameController(std::vector<std::unique_ptr<Player>>& players,
    Board& board, Dice& dice, GameViewInterface& view,
    CommandInterface& command)
    : players_(players), board_(board), dice_(dice),
    view_(view), command_(command) {}

GameController::~GameController() = default;

void GameController::playGame(int latestTurn, int maxTurn) {
    int i = latestTurn;
    while (i < maxTurn && !hasSoleWinner()) {
        for (auto& player : players_) {
            if (!player->isBankrupt()) {
                processTurn(*player);
            }
        }
        i++;
    }

    // cek pemenangnya satu doang atau banyak
}

bool GameController::hasSoleWinner() const{
    size_t countNotBankrupt = 0;
    for (auto& player : players_) {
        if (!player->isBankrupt()) {
            countNotBankrupt++;
        }
    }
    return countNotBankrupt == players_.size() - 1;
}

// RESTRIKSI : di luar on land / effect kartu aja la baru bs bolak balik:
void GameController::processTurn(Player& p) {
    view_.showMessage("Turn " + p.getUsername() + " dimulai!\n");

    // Pick & Drop Card Kemampuan
    processPickAndDropSpecialCard(p);

    if (p.isInJail() && !p.thisTurnAutoFreeFromJail()) {
        processJailTurn(p);
        return;
    } else if (p.isInJail() && p.thisTurnAutoFreeFromJail()) {
        // bayar denda
        // ya cari lah itu biaya penjara sisanya kyk biasa aja
    }
    processNormalTurn(p);
}

void GameController::processPickAndDropSpecialCard(Player& p) {
    // sesuaikan di spek
    // SkillCard& newcard = deck.drawDeck();
    view_.showMessage("Kamu mendapatkan 1 kartu acak baru!\nKartu yang di dapat adalah.." );
    // masukkan ke inventory player
    // tampilin juga daftarnya...
    // command_->getInt(0, total kartu)

    view_.showMessage("Kartu x telah dibuang ....");
}

void GameController::processSpecialCardUse(Player& p) {
    view_.showMessage("Silahkan pilih kartu yang mau kamu pakai!\n");
    // cek apakah kartu yang dipakai bisa atau ga bs

    // Sselama di Penjara,
    // pemain tidak dapat bergerak (termasuk pergerakan yang di-invoke 
    // dari kartu seperti move dan teleport). Untuk kartu lainnya, kasus
    // penggunaannya sama seperti biasa.
}

void GameController::processRollDice(Player& p) {
    view_.showMessage("Silahkan roll dice kamu!\n");


}

void GameController::processNormalTurn(Player& p) {
    bool hasRolledFirsTime = false;
    bool canSave = true;

    // Command
    while (p.notViolatingDoubleRollCount() && !p.isBankrupt() && !p.isInJail()) {
        // dia bisa tiba-tiba jadi di penjara artinya berhenti udh (baru masuk)
        if (p.isInJail()) {
            break;
        }
        Command cmd = command_.getCommand();

        switch (cmd.getType()) {
            case CommandType::CETAK_PAPAN:
                view_.cetakPapan();
                break;
            case CommandType::LEMPAR_DADU:
                hasRolledFirsTime = true;

                break;
            case CommandType::ATUR_DADU:
                hasRolledFirsTime = true;
                //
                break;
            case CommandType::CETAK_AKTA:
                view_.cetakAkta();
                break;
            case CommandType::CETAK_PROPERTI:
                view_.cetakProperti();
                break;
            case CommandType::GADAI:
                processMortgage();
                //
                break;
            case CommandType::TEBUS:
                processRedeem();
                //
                break;
            case CommandType::BANGUN:
                processBuyBuilding();
                break;
            case CommandType::SIMPAN:
                // simpan..
                break;
            case CommandType::MUAT:
                // langsung pesan error sajah
                break;
            case CommandType::CETAK_LOG:
                // cetak semua lognya
                break;
            case CommandType::GUNAKAN_KEMAMPUAN:
                if (!hasRolledFirsTime) {
                    processSpecialCardUse(p);
                } else {
                    view_.showMessage("Special card hanya dapat dipakai sebelum roll dice pertama kali");
                }
                break;
            
            default:
                break;
        }
        canSave = false;
    }

    // cek apakah dia violate double itu / dia bankrupt / emg udh masuk aja ke penjara
    // make sure kalo dia violate double itu bakal lgsg masuk penjara
}

void GameController::processJailTurn(Player& p) {
    // udah pasti either bayar / atur" dadud
    view_.showMessage("Anda sedang berada di penjara!\n");
    Command cmd = command_.getCommand();

    switch (cmd.getType()) {
        case CommandType::GUNAKAN_KEMAMPUAN:
            // ingat restriksi ketika dia di dalam penjara
            processSpecialCardUse(p);
            break;
        case CommandType::LEMPAR_DADU:
            // lempar dadu trs kalo double baru keluar
            break;
        case CommandType::ATUR_DADU:
            // set dadu trs kalo double baru keluar
            break;
        case CommandType::BAYAR_DENDA:
            // bayar terus lgsg keluar ke normal turn
            processNormalTurn(p);
            break;
        default:
            break;
    }
}

// Bisa di-consider perlu kelas sendiri "AuctionController" atau engga
void GameController::processAuction(Player& p, PropertyTile& propertyTile) {
    // Kumpulin siapa aja yang ikut lelang
    // Lanjutin sesuai spek :)
}

void GameController::processMovement(Player& p, int firstDisplacement) {
    Tile& nextTile = board_.moveToNextTile(p.move(firstDisplacement));

    // proses di dalam land
    p.setPosition(nextTile.getTileID());
    OnLandResult result = nextTile.onLand(p, command_, view_);

    switch (result) {
        // PROBLEM : KALO orang lain bankrut karena Lasso card gimana dong
        case OnLandResult::TriggerAuction: // Ketika ga beli properti saja
            // Kasus kalo emang lagi di land property
            // Pemain menolak membeli properti
            // Pemain tidak mampu membeli

            // Tapi kasus kalo bankrut gimana dong,
            // bisa jadi ga lagi di propertnya dia
            // bisa jadi malah bukan giliran orang bersangkutan
            // karena lasso card

            // processAuction(p, nextTile);
            break;
        case OnLandResult::TriggerBankruptcyAuction: //
            // Pemain bangkrut ke Bank (semua properti dilelang)
                // Penyebab bankrut :
                // tidak mampu memenuhi kewajiban pembayaran
                // baik berupa sewa, pajak, 
                // maupun efek dari kartu tertentu
            // posisinya lagi di player sekarang ga si hrsnya tapi si lasso card ni gmnn weeee
        case OnLandResult::TakeChanceCard:
            processTakeChanceCard(p);
            break;
        case OnLandResult::TakeCommunityChest:
            processTakeCommunityChest(p);
            break;
        case OnLandResult::Done: // yang kelar
            break;
        case OnLandResult::TriggerMoveToJail: // move to jail
            p.setPosition(board_.getJailPosition());
            view_.showMessage("Kamu dipindahkan ke penjara");
            break;
            // go to jail perlu?
        default:
            break;
    }
}

void GameController::processTakeChanceCard(Player& p) {
    // Ngambil kartu dari deck
    // Kartu Kesempatan
    // Berikut adalah aturan Kartu Kesempatan yang ada dalam Permainan Nimonspoli:
    // "Pergi ke stasiun terdekat."
    // "Mundur 3 petak."
    // "Masuk Penjara."
}

void GameController::processTakeCommunityChest(Player& p) {
    // Ngambill kartu dari deck
    // pake effect dari kartunya
    // "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain."
    // "Biaya dokter. Bayar M700."
    // "Anda mau nyaleg. Bayar M200 kepada setiap pemain."

    // CommunityChest parameternya hanya boleh pemain (yang punya) dan juga semua player
}