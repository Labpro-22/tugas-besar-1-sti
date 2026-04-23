#include "controllers/GameController.hpp"
#include "models/card/skillcard/MoveCard.hpp"
#include "models/card/skillcard/TeleportCard.hpp"

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
    bool hasUsedSkillCardThisTurn = false;

    if (p.isInJail() && !p.thisTurnAutoFreeFromJail()) {
        processJailTurn(p, hasUsedSkillCardThisTurn);
        p.consumeShield();
        return;
    } else if (p.isInJail() && p.thisTurnAutoFreeFromJail()) {
        // bayar denda
        // ya cari lah itu biaya penjara sisanya kyk biasa aja
    }
    processNormalTurn(p, hasUsedSkillCardThisTurn);
    p.consumeShield();
}

void GameController::processPickAndDropSpecialCard(Player& p) {
    (void)p;
}

void GameController::processSpecialCardUse(Player& p, bool& hasUsedSkillCardThisTurn) {
    if (hasUsedSkillCardThisTurn) {
        view_.showMessage("Kartu kemampuan hanya boleh dipakai maksimal 1 kali per giliran.\n");
        return;
    }

    if (p.getSkillCardCount() == 0) {
        view_.showMessage("Kamu tidak punya kartu kemampuan untuk digunakan.\n");
        return;
    }

    view_.showMessage("Silahkan pilih kartu yang mau kamu pakai!\n");
    for (std::size_t i = 0; i < p.getSkillCardCount(); ++i) {
        const SkillCard* card = p.getSkillCardAt(i);
        view_.showMessage(std::to_string(i + 1) + ". " + card->getName() + " - " + card->getDescription() + "\n");
    }

    int selected = command_.getInt(1, static_cast<int>(p.getSkillCardCount()));
    std::size_t selectedIndex = static_cast<std::size_t>(selected - 1);
    const SkillCard* selectedCard = p.getSkillCardAt(selectedIndex);

    if (p.isInJail()) {
        if (dynamic_cast<const MoveCard*>(selectedCard) != nullptr ||
            dynamic_cast<const TeleportCard*>(selectedCard) != nullptr) {
            view_.showMessage("Saat di penjara, kartu Move/Teleport tidak dapat digunakan.\n");
            return;
        }
    }

    std::unique_ptr<SkillCard> usedCard = p.takeSkillCard(selectedIndex);
    usedCard->activate(p);
    hasUsedSkillCardThisTurn = true;
    view_.showMessage("Kartu " + usedCard->getName() + " telah dipakai.\n");

    // Sselama di Penjara,
    // pemain tidak dapat bergerak (termasuk pergerakan yang di-invoke 
    // dari kartu seperti move dan teleport). Untuk kartu lainnya, kasus
    // penggunaannya sama seperti biasa.
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
            view_.showMessage("Player tidak mampu membayar ke Bank!\n");

            std::vector<PropertyTile*> properties = p.getProperties();

            auction_.processBankruptcyToBank(p);

            // kalau tidak ada property 
            if (properties.empty()) {
                break;
            }

            for (PropertyTile* property : properties) {
                if (property == nullptr) continue;

                view_.showMessage("LELANG");

                // mulai auction
                auction_.start(p, *property, Auction::AuctionCause::BANKRUPTCY_TO_BANK);

                while (!auction_.isFinished()) {
                    Player* current = auction_.getCurrentPlayer();

                    if (current == nullptr) {
                        break;
                    }

                    view_.showMessage("\nGiliran: " + current->getUsername() + "\n");

                    int minBid = auction_.getMinBid();

                    // cek apakah boleh pass
                    if (auction_.canCurrentPlayerPass()) {
                        view_.showMessage("Aksi:\n");
                        view_.showMessage("0 -> PASS\n");
                        view_.showMessage(std::to_string(minBid) + "+ -> BID\n");
                    } else {
                        view_.showMessage("Wajib BID (tidak boleh PASS)\n");
                        view_.showMessage("Masukkan bid >= " + std::to_string(minBid) + "\n");
                    }

                    view_.showMessage("Saldo: M" + std::to_string(current->getBalance()) + "\n");
                    view_.showMessage("Input: ");

                    int input = command_.getInt(0, current->getBalance());

                    if (input == 0) {
                        if (auction_.canCurrentPlayerPass()) {
                            auction_.passCurrentPlayer();
                        } else {
                            view_.showMessage("Tidak boleh PASS di giliran ini!\n");
                        }
                    } else {
                        if (auction_.canCurrentPlayerBid(input)) {
                            auction_.bidCurrentPlayer(input);
                        } else {
                            view_.showMessage("Bid tidak valid!\n");
                        }
                    }
                }

                view_.showMessage("\nLelang selesai.\n");
            }

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