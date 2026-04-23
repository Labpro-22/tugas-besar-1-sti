#include "controllers/GameController.hpp"
#include "models/card/skillcard/MoveCard.hpp"
#include "models/card/skillcard/TeleportCard.hpp"

GameController::GameController(std::vector<std::unique_ptr<Player>> players,
    Board& board, Dice& dice, GameViewInterface& view,
    CommandInterface& command)
    : players_(players), board_(board), dice_(dice),
    view_(view), command_(command), auction_(players_, view_){}

GameController::~GameController() = default;

// TERKAIT LOGIC GAME SECARA UMUM =========================================================================================================
void GameController::playGame(int latestTurn, int maxTurn) {
    int i = latestTurn;
    while ((i < maxTurn || maxTurn == -1) && !hasSoleWinner()) {
        for (auto& player : players_) {
            if (!player->isBankrupt()) {
                processTurn(*player);
            }
        }
        i++;
    }
    decideWinner();
}

void GameController::decideWinner() const {
    // Pemenang adalah pemain dengan uang terbanyak.
    // Jika seri, pemain yang memiliki jumlah petak properti
        // terbanyak yang menjadi pemenang.
    // Jika seri, pemain yang memiliki jumlah
        // kartu terbanyak yang menjadi pemenang.
    // Jika seri, semua pemain yang masih
        //seri menjadi pemenang.
    // Jika kondisi bankruptcy terjadi sebelum
        // mencapai batas maksimum giliran,
        //ikuti aturan bankruptcy.
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

// TERKAIT TURN =========================================================================================================
// RESTRIKSI : di luar on land / effect kartu aja la baru bs bolak balik:
void GameController::processTurn(Player& p) {
    view_.showMessage("Turn " + p.getUsername() + " dimulai!\n");
    bool hasUsedSkillCardThisTurn = false;

    if (p.isInJail() && !p.thisTurnAutoFreeFromJail()) {
        processJailTurn(p, hasUsedSkillCardThisTurn);
        p.decreaseShieldCardTurn();
        return;
    } else if (p.isInJail() && p.thisTurnAutoFreeFromJail()) {
        view_.showMessage("Round ini, kamu auto free!\nBayar denda!\n");
        p.leaveJail();
        p.deductMoney(jailFine_);
    }
    processNormalTurn(p, hasUsedSkillCardThisTurn);
    p.decreaseShieldCardTurn();
}

void GameController::processRollDice(Player& p) {
    view_.showMessage("Silahkan roll dice kamu!\n");
}

void GameController::processNormalTurn(Player& p, bool& hasUsedSkillCardThisTurn) {
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
                processMortgage(p);
                //
                break;
            case CommandType::TEBUS:
                processRedeem(p);
                //
                break;
            case CommandType::BANGUN:
                processBuyBuilding(p);
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
                    processSpecialCardUse(p, hasUsedSkillCardThisTurn);
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

void GameController::processJailTurn(Player& p, bool& hasUsedSkillCardThisTurn) {
    // udah pasti either bayar / atur" dadud
    view_.showMessage("Anda sedang berada di penjara!\n");
    Command cmd = command_.getCommand();

    switch (cmd.getType()) {
        case CommandType::GUNAKAN_KEMAMPUAN:
            // ingat restriksi ketika dia di dalam penjara
            processSpecialCardUse(p, hasUsedSkillCardThisTurn);
            break;
        case CommandType::LEMPAR_DADU:
            // lempar dadu trs kalo double baru keluar
            break;
        case CommandType::ATUR_DADU:
            // set dadu trs kalo double baru keluar
            break;
        case CommandType::BAYAR_DENDA:
            // bayar terus lgsg keluar ke normal turn
            // perlu try n catch??
            p.deductMoney(jailFine_);
            p.leaveJail();
            processNormalTurn(p, hasUsedSkillCardThisTurn);
            break;
        default:
            break;
    }
}

void GameController::processMovement(Player& p, int firstDisplacement) {
    Tile& nextTile = board_.moveToNextTile(p.move(firstDisplacement));

    // proses di dalam land
    p.setPosition(nextTile.getTileID());
    p.setLastDiceTotal(firstDisplacement);
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
        case OnLandResult::TriggerBankruptcyAuction: {
            state_ = GameState::BANKRUT;

            auction_.runBankruptcyAuction(p, command_);

            state_ = GameState::WAITING_FOR_ROLL_DICE;
            break;
        }
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
        case OnLandResult::Festival:
            processFestival(p);
            break;
        case OnLandResult::Done: // yang kelar
            break;
        case OnLandResult::TriggerMoveToJail: // move to jail
            p.setPosition(board_.getJailPosition());
            view_.showMessage("Kamu dipindahkan ke penjara");
            break;
        default:
            break;
    }
}

// pasti udh ada properti
void GameController::processFestival(Player& p) {
    view_.showMessage("Daftar properti milikmu...\n");

    // cek valid atau engga
    std::string chosenPropCode;
    PropertyTile* propTile = nullptr; // nantilah ubah malas
    do {
        view_.showMessage("Masukkan kode properti untuk festival: ");
        Command cmd = command_.getCommand();
        chosenPropCode = cmd.getStringArg();
        if (!board_.has(chosenPropCode)) {
            // continue
            view_.showMessage("Kode tidak valid!\n");
            continue;

        }

        if (!p.hasProperty(chosenPropCode)) {
            view_.showMessage("Tetot\n");
            continue;
        }

        propTile = &p.getProperty(chosenPropCode);

        if (!propTile->canDoubleFestival()) {
            view_.showMessage("Ga bs double lagi woyyy\n");
            propTile = nullptr;
            continue;
        }

    } while (propTile == nullptr); // tar jgn pake nullptr la cek aja lg

    if (propTile->festivalActive()) {
        if (propTile->canDoubleFestival()) {
            // lgsg double aja
            if (!propTile->alreadyMaxMultiplier()) {
                propTile->doubleTheMultiplier();
            }
            propTile->resetTurnTo3();
        }
    } else {
        propTile->doubleTheMultiplier();
        propTile->resetTurnTo3();
    }

    view_.showMessage("Hihi hiha!\n");
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

void GameController::processMortgage(Player& p) {

}

void GameController::processRedeem(Player& p) {
    std::vector<PropertyTile *> mortgagedProperties = p.getMortgagedProperties();
    if (mortgagedProperties.empty()) {
        view_.showMessage("KOSONG GA ADA WOII!\n");
        return;
    }

    view_.showMessage("Berikut ini properti yang sedang digadaikan!\n");
    int toBeRedeemed = command_.getInt(0, mortgagedProperties.size());

    if (toBeRedeemed == 0) {
        view_.showMessage("Cancels");
        return;
    }

    if (p.getBalance() < mortgagedProperties.at(toBeRedeemed - 1)->getMortgageValue()) {
        // gagals
        view_.showMessage("Gagals");
        return;
    }

    p.deductMoney(mortgagedProperties.at(toBeRedeemed - 1)->getMortgageValue());
    mortgagedProperties.at(toBeRedeemed - 1)->setPropertyStatus(PropertyStatus::OWNED);

    view_.showMessage("Yip yip show message sisa uangnya sama prop yg bs ditebus juga!\n");
}

void GameController::processBuyBuilding(Player& p) {

}