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

void GameController::transferProperty(Player& from, Player& to, PropertyTile& propertyTile) {
    PropertyStatus oldStatus = propertyTile.getPropertyStatus();

    from.removeProperty(&propertyTile);
    to.addProperty(&propertyTile);

    propertyTile.setPropertyStatus(oldStatus);
}

// Bisa di-consider perlu kelas sendiri "AuctionController" atau engga
void GameController::processAuction(Player& triggerPlayer, PropertyTile& propertyTile) {
    state_ = GameState::LELANG;

    view_.showMessage("Properti " + propertyTile.getTileName() + " (" + propertyTile.getLetterCode() + ") akan dilelang!\n");

    // cari index trigger player
    int triggerIdx = -1;
    for (size_t i = 0; i < players_.size(); i++) {
        if (players_[i].get() == &triggerPlayer) {
            triggerIdx = static_cast<int>(i);
            break;
        }
    }

    if (triggerIdx == -1) {
        view_.showMessage("Trigger player tidak ditemukan. Lelang dibatalkan.\n");
        state_ = GameState::WAITING_FOR_ROLL_DICE;
        return;
    }

    // untuk peserta lelang: udah sesuai urutan
    // kalo trigger player gak bankrupt -> ikut putaran terakhir
    // kalo bakrupt -> ga ikut
    std::vector<Player*> participants;
    int n = static_cast<int>(players_.size());

    for (int step = 1; step <= n; step++) {
        int idx = (triggerIdx + step) % n;
        Player* candidate = players_[idx].get();

        if (candidate->isBankrupt()) {
            continue;
        }

        if (candidate == &triggerPlayer && triggerPlayer.isBankrupt()) {
            continue;
        }

        participants.push_back(candidate);
    }

    if (participants.empty()) {
        view_.showMessage("Tidak ada peserta valid untuk lelang.\n");
        state_ = GameState::WAITING_FOR_ROLL_DICE;
        return;
    }

    view_.showMessage("Urutan lelang dimulai dari pemain setelah " + triggerPlayer.getUsername() + ".\n");

    int highestBid = -1;
    Player* currentWinner = nullptr;
    int consecutivePasses = 0;
    int idx = 0;

    while (true) {
        Player* currentPlayer = participants[idx];
        view_.showMessage("\nGiliran: " + currentPlayer->getUsername() + "\n");

        bool forcedBid = (currentWinner == nullptr && consecutivePasses == static_cast<int>(participants.size()) - 1);

        if (forcedBid) {
            view_.showMessage("Semua pemain sebelumnya PASS. Kamu wajib melakukan bid.\n");
            view_.showMessage("Masukkan bid (minimal 0, maksimal " + std::to_string(currentPlayer->getBalance()) + "): ");
            int bid = command_.getInt(0, currentPlayer->getBalance());

            highestBid = bid;
            currentWinner = currentPlayer;
            consecutivePasses = 0;

            view_.showMessage("Penawaran tertinggi: M" + std::to_string(highestBid) +
                              " (" + currentWinner->getUsername() + ")\n");
        } else {
            int minBid;
            if (highestBid < 0) {
                minBid = 0;
            } else {
                minBid = highestBid + 1;
            }
            view_.showMessage("Aksi auction:\n");
            view_.showMessage("- masukkan 0 untuk PASS\n");
            view_.showMessage("- masukkan angka >= " + std::to_string(minBid) + " untuk BID\n");
            view_.showMessage("Masukkan pilihan (maksimal " + std::to_string(currentPlayer->getBalance()) + "): ");

            int input = command_.getInt(0, currentPlayer->getBalance());

            if (input == 0) {
                view_.showMessage(currentPlayer->getUsername() + " memilih PASS.\n");
                consecutivePasses++;

                if (currentWinner != nullptr &&
                    consecutivePasses >= static_cast<int>(participants.size()) - 1) {
                    break;
                }
            } else {
                if (input < minBid) {
                    view_.showMessage("Bid tidak valid karena harus lebih tinggi dari bid sebelumnya.\n");
                    // giliran pemain yang sama diulang
                    continue;
                }

                highestBid = input;
                currentWinner = currentPlayer;
                consecutivePasses = 0;

                view_.showMessage("Penawaran tertinggi: M" + std::to_string(highestBid) +
                                  " (" + currentWinner->getUsername() + ")\n");
            }
        }

        idx = (idx + 1) % static_cast<int>(participants.size());
    }

    view_.showMessage("\nLelang selesai!\n");

    if (currentWinner != nullptr) {
        currentWinner->deductMoney(highestBid);
        currentWinner->addProperty(&propertyTile);

        view_.showMessage("Pemenang: " + currentWinner->getUsername() + "\n");
        view_.showMessage("Harga akhir: M" + std::to_string(highestBid) + "\n");
        view_.showMessage("Properti " + propertyTile.getTileName() + " (" + propertyTile.getLetterCode() +
                          ") kini dimiliki " + currentWinner->getUsername() + ".\n");
    } else {
        view_.showMessage("Tidak ada pemenang lelang.\n");
    }

    state_ = GameState::WAITING_FOR_ROLL_DICE;
}

void GameController::processBankruptcyToBank(Player& p) {
    state_ = GameState::BANKRUT;

    view_.showMessage("\n" + p.getUsername() + " dinyatakan BANGKRUT kepada Bank!\n");

    int remainingMoney = p.getBalance();
    if (remainingMoney > 0) {
        view_.showMessage("Uang sisa M" + std::to_string(remainingMoney) + " diserahkan ke Bank.\n");
        p.deductMoney(remainingMoney);
    }

    std::vector<PropertyTile*> properties = p.getProperties();

    if (properties.empty()) {
        view_.showMessage("Pemain tidak memiliki properti untuk dilelang.\n");
    } else {
        view_.showMessage("Seluruh properti dikembalikan ke Bank dan akan dilelang satu per satu.\n");
    }

    for (PropertyTile* property : properties) {
        if (property == nullptr) {
            continue;
        }

        // pepas dari inventory 
        p.removeProperty(property);

        // reset jadi milik bank
        property->resetAfterBankruptcyToBank();

        view_.showMessage("\n-> Lelang: " + property->getTileName() +
                          " (" + property->getLetterCode() + ")\n");

        processAuction(p, *property);
    }

    p.setStatus(Player::PlayerStatus::BANKRUPT);

    view_.showMessage(p.getUsername() + " telah keluar dari permainan.\n");
    state_ = GameState::WAITING_FOR_ROLL_DICE;
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
        case OnLandResult::TriggerBankruptcyAuction: 
            PropertyTile* propertyTile = dynamic_cast<PropertyTile*>(&nextTile);

            if (propertyTile != nullptr) {
                processAuction(p, *propertyTile);
            } else {
                processBankruptcyToBank(p);
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
        chosenPropCode = command_.getTileToGetFestival();
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