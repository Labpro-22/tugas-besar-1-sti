#include "controllers/GameController.hpp"
#include "models/card/skillcard/MoveCard.hpp"
#include "models/card/skillcard/TeleportCard.hpp"

GameController::GameController(std::vector<std::unique_ptr<Player>> players,
    Board& board, Dice& dice, GameViewInterface& view,
    CommandInterface& command, Deck<SkillCard>& specialCardDeck)
    : players_(std::move(players)), board_(board), dice_(dice),
    view_(view), command_(command), specialCardDeck_(specialCardDeck) {}

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
    std::vector<Player*> candidates;
    for (const auto& playerPtr : players_) {
        if (playerPtr != nullptr && !playerPtr->isBankrupt()) {
            candidates.push_back(playerPtr.get());
        }
    }
    if(hasSoleWinner()){
        for (const auto& playerPtr : players_) {
            if (!playerPtr->isBankrupt()) {
                view_.showMessage("Pemenang: " + playerPtr->getUsername() + "\n");
                view_.showMessage("Uang: M" + std::to_string(playerPtr->getBalance()) + "\n");
                return;
            }
        }
    }
    if (candidates.empty()) {
        view_.showMessage("Tidak ada pemenang karena semua pemain bangkrut.\n");
        return;
    }

    // uang terbanyak
    int maxBalance = candidates[0]->getBalance();
    for (Player* player : candidates) {
        if (player->getBalance() > maxBalance) {
            maxBalance = player->getBalance();
        }
    }
    // cari player yang balance nya = maxBalance
    std::vector<Player*> balanceWinners;
    for (Player* player : candidates) {
        if (player->getBalance() == maxBalance) {
            balanceWinners.push_back(player);
        }
    }
    // kalo cuma 1 yang uangnya paling banyak
    if (balanceWinners.size() == 1) {
        view_.showMessage("Pemenang: " + balanceWinners[0]->getUsername() + "\n");
        view_.showMessage("Uang: M" + std::to_string(balanceWinners[0]->getBalance()) + "\n");
        return;
    }

    // property terbanyak
    int maxPropertyCount = balanceWinners[0]->getPropertyCount();
    for (Player* player : balanceWinners) {
        if (player->getPropertyCount() > maxPropertyCount) {
            maxPropertyCount = player->getPropertyCount();
        }
    }
    // cari player yang property count nya = maxPropertyCount
    std::vector<Player*> propertyWinners;
    for (Player* player : balanceWinners) {
        if (player->getPropertyCount() == maxPropertyCount) {
            propertyWinners.push_back(player);
        }
    }
    // kalo cuma 1 yang properti nya paling banyak
    if (propertyWinners.size() == 1) {
        view_.showMessage("Pemenang: " + propertyWinners[0]->getUsername() + "\n");
        view_.showMessage("Uang: M" + std::to_string(propertyWinners[0]->getBalance()) + "\n");
        view_.showMessage("Jumlah properti: " + std::to_string(propertyWinners[0]->getPropertyCount()) + "\n");
        return;
    }

    // kartu terbanyak
    int maxCardCount = propertyWinners[0]->getCardCount();
    for (Player* player : propertyWinners) {
        if (player->getCardCount() > maxCardCount) {
            maxCardCount = player->getCardCount();
        }
    }
    // cari player yang card count nya = maxCardCount
    std::vector<Player*> finalWinners;
    for (Player* player : propertyWinners) {
        if (player->getCardCount() == maxCardCount) {
            finalWinners.push_back(player);
        }
    }

    // pemenang
    if (finalWinners.size() == 1) {
        view_.showMessage("Pemenang: " + finalWinners[0]->getUsername() + "\n");
        view_.showMessage("Uang: M" + std::to_string(finalWinners[0]->getBalance()) + "\n");
        view_.showMessage("Jumlah properti: " + std::to_string(finalWinners[0]->getPropertyCount()) + "\n");
        view_.showMessage("Jumlah kartu: " + std::to_string(finalWinners[0]->getCardCount()) + "\n");
    } else { // size != 1
        view_.showMessage("Permainan berakhir seri. Para pemenang:\n");
        for (Player* player : finalWinners) {
            view_.showMessage("- " + player->getUsername() +
                              " | Uang: M" + std::to_string(player->getBalance()) +
                              " | Properti: " + std::to_string(player->getPropertyCount()) +
                              " | Kartu: " + std::to_string(player->getCardCount()) +
                              "\n");
        }
    }
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

bool GameController::processRandomDice(Player& p){
    view_.showMessage("Mengocok dadu...\n");
    dice_.roll();
    return resolveDiceResult(p, dice_.getDie1(), dice_.getDie2());
}
bool GameController::processCustomDice(Player& p, int x, int y){
    if (x < 1 || x > 6 || y < 1 || y > 6) {
        view_.showMessage("Nilai dadu harus antara 1 sampai 6.\n");
        return false;
    }
    view_.showMessage("Dadu diatur secara manual.\n");
    dice_.rollSettingan(x, y);
    return resolveDiceResult(p, dice_.getDie1(), dice_.getDie2());
}
bool GameController::resolveDiceResult(Player& p, int d1, int d2){
    int total = d1 + d2;
    view_.showMessage("Hasil: " + std::to_string(d1) + " + " + std::to_string(d2) + " = " + std::to_string(total) + "\n");
    if (d1 == d2) {
        p.incrementDoubleCount();
        if (!p.notViolatingDoubleRollCount()) {
            view_.showMessage("Triple double! Masuk penjara.\n");
            p.setStatus(Player::PlayerStatus::JAILED);
            p.resetJailTurn();
            p.setPosition(board_.getJailPosition());
            return false;
        }
        processMovement(p, total);
        return true; 
    } 
    else {
        p.resetCountDouble();
        processMovement(p, total);
        return false;
    }
}

void GameController::processNormalTurn(Player& p, bool& hasUsedSkillCardThisTurn) {
    bool hasRolledFirstTime = false;
    bool canSave = true;
    bool continueTurn = false; // untuk ngecek apakah dia dapat roll lagi karena double atau engga

    // Command
    while (continueTurn && !p.isBankrupt() && !p.isInJail()) {
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
                hasRolledFirstTime = true;
                continueTurn = processRandomDice(p);
                if (continueTurn && !p.isInJail() && !p.isBankrupt()) {
                    view_.showMessage("Kamu mendapatkan double! Kamu bisa roll lagi!\n");
                }

                break;
            case CommandType::ATUR_DADU: 
                if (cmd.getArgCount() < 2) {
                    view_.showMessage("Format ATUR_DADU salah. Gunakan: ATUR_DADU X Y\n");
                    break;
                }
                hasRolledFirstTime = true;
                int x = cmd.getArg(0);
                int y = cmd.getArg(1);
                continueTurn = processCustomDice(p, x, y);
                if (continueTurn && !p.isInJail() && !p.isBankrupt()) {
                    view_.showMessage("Kamu mendapatkan double! Kamu bisa roll lagi!\n");
                }

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
                if (!hasRolledFirstTime) {
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

// PROCES SKILL CARD USAGE ====================================================================================================
void GameController::processPickAndDropSpecialCard(Player& p) {
    std::unique_ptr<SkillCard> newObtainedCard = specialCardDeck_.drawDeck();
    view_.showMessage("KARTU YANG DIDAPATKAN ..........");
    bool throwNewObtained = false; // latest
    int selected = p.getSkillCardCount() + 1;

    try {
        // push ke dalam stack
        p.getInventory().addSkillCards(std::move(newObtainedCard));
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n'; ///////
        view_.showMessage("Silahkan pilih yang mau dibuang terlebih dahulu\n");

        int selected = command_.getInt(1,  p.getSkillCardCount() + 1);
        if (selected  == p.getSkillCardCount() + 1) {
            throwNewObtained = true;
        }

        if (throwNewObtained) {
            // Balikin ke deck aja
            specialCardDeck_.pushToDiscard(std::move(newObtainedCard));
        } else {
            std::unique_ptr<SkillCard> thrownAwayCard = p.getInventory().removeSkillCardAt(selected);
            p.getInventory().addSkillCards(std::move(newObtainedCard));
            specialCardDeck_.pushToDiscard(std::move(thrownAwayCard));
        }
        return;
    }

    // bisa langsung push
    p.getInventory().addSkillCards(std::move(newObtainedCard));
    view_.showMessage("Kartu X berhasil dibuang, Kartu Y kembali diterima!\n");
}

/*
Memproses penggunaan skill card
Notes : MoveCard, LassoCard, TeleportCard
*/
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

    // NOTES : ga perlu pake loop kirim aja biar si interface yg ngurus
    // for (std::size_t i = 0; i < p.getSkillCardCount(); ++i) {
    //     const SkillCard* card = p.getSkillCardAt(i);
    //     view_.showMessage(std::to_string(i + 1) + ". " + card->getName() + " - " + card->getDescription() + "\n");
    // }

    int selected = command_.getInt(1, p.getSkillCardCount());
    std::size_t selectedIndex = static_cast<std::size_t>(selected - 1);
    const SkillCard* selectedCard = p.getSkillCardAt(selectedIndex);

    if (p.isInJail()) {
        if (dynamic_cast<const MoveCard*>(selectedCard) != nullptr ||
            dynamic_cast<const TeleportCard*>(selectedCard) != nullptr ||
            dynamic_cast<const LassoCard*>(selectedCard) != nullptr) {
            view_.showMessage("Saat di penjara, kartu Move/Teleport tidak dapat digunakan.\n");
            return;
        }
    }

    std::unique_ptr<SkillCard> usedCard = p.removeSkillCardAt(selectedIndex);
    usedCard->activate(p);
    // masukkan balik dong ke deck-nya
    view_.showMessage("Kartu " + usedCard->getName() + " telah dipakai.\n");

    specialCardDeck_.pushToDiscard(std::move(usedCard));
    hasUsedSkillCardThisTurn = true;
}

// AUCTION RELATED ========================================================================================
void GameController::transferProperty(Player& from, Player& to, PropertyTile& propertyTile) {
    PropertyStatus oldStatus = propertyTile.getPropertyStatus();

    from.removeProperty(&propertyTile);
    to.addProperty(&propertyTile);

    propertyTile.setPropertyStatus(oldStatus);
}

// Bisa di-consider perlu kelas sendiri "AuctionController" atau engga
void GameController::processAuction(Player& triggerPlayer, PropertyTile& propertyTile) {
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
}

void GameController::processBankruptcyToBank(Player& p) {
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
}

// TILE RELATED ========================================================================================
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
            auction_.runBankruptcyAuction(p, command_);
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
        case OnLandResult::TriggerMoveToJail: // move to jail
            p.setPosition(board_.getJailPosition());
            view_.showMessage("Kamu dipindahkan ke penjara");
            break;
        case OnLandResult::TriggerTryToPayRent:
            processPayRent(p, nextTile);
        case OnLandResult::Done: // yang kelar
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

    if (p.getBalance() < mortgagedProperties.at(toBeRedeemed - 1)->getPurchasePrice()) {
        // gagals
        view_.showMessage("Gagals");
        return;
    }

    p.deductMoney(mortgagedProperties.at(toBeRedeemed - 1)->getPurchasePrice());
    mortgagedProperties.at(toBeRedeemed - 1)->setPropertyStatus(PropertyStatus::OWNED);

    view_.showMessage("Yip yip show message sisa uangnya sama prop yg bs ditebus juga!\n");
}

void GameController::processBuyBuilding(Player& p) {

}

// sudah pasti ga mortgage dan sudah pasti bisa bayar, sisa pindahin uang
void GameController::processPayRent(Player& p, Tile& currentTile) {
    // get owner
    PropertyTile* propertyTile = dynamic_cast<PropertyTile*>(&currentTile);
    if (!propertyTile) {
        return;
    }

    for (size_t i = 0; i < players_.size(); i++) {
        Player* owner = players_[i].get();
        if (owner && owner->getUsername() == propertyTile->getOwnerUsername()) {
            int rent = 0;
            if (StreetTile* streetTile = dynamic_cast<StreetTile*>(propertyTile)) {
                bool cg = board_.isCompletedColourGroup(owner->getUsername(), propertyTile->getColourBlock());
                rent = streetTile->calculateRentPrice(cg);
            }
            else if (RailRoadTile* railRoadTile = dynamic_cast<RailRoadTile*>(propertyTile)) {
                int count = board_.countOwnedRailRoadTile(owner->getUsername());
                rent = railRoadTile->calculateRentPrice(count);
            }
            else if (UtilityTile* utilityTile = dynamic_cast<UtilityTile*>(propertyTile)) {
                int count = board_.countOwnedUtilityTile(owner->getUsername());
                int dice = dice_.getRollResult(); // tetap sm kyk terakhir selagi ga pernah di-roll lg
                rent = utilityTile->calculateRentPrice(count, dice);
            }
            if (p.getBalance() < rent) {
                processBankruptcyFlow(p, *owner);
                return;
            }
            // bayar beneran
            owner->addMoney(rent);
            p.deductMoney(rent);

            view_.showMessage("Atur-atur mo tampilannya gimana :VVVVVVVVVVVVVVVVVV");
            return;
        }
    }
    
}

// nantilah aaaaaaaaaaaaaaaaaaaaaaaaaaa mslh cari data buat view-nya
void GameController::processBuyBuilding(Player& p) {
    view_.showMessage("ini ni yg memenuhi syarat");

    // Cek user punya petak yang memenuhi atau engga
    std::map<std::string, std::vector<PropertyTile*>> completeColourGroup = p.getCompleteColourGroups(board_.getCountTilesForEachColourBlock());
    if (completeColourGroup.empty()) {
        view_.showMessage("Tidak ada color group yang memenuhi syarat untuk dibangun\n");
        return;
    }
    view_.showMessage("Nah ini dia bla bla bla bla bla");

    // tampilin uang saat ini, pilih mana colour group mana yang mau
    int selected = command_.getInt(0, completeColourGroup.size());

    if (selected == 0) {
        return;
    }

    // tampilin dari colour group yang mau dipilih
    auto it = std::next(completeColourGroup.begin(), selected - 1);

    std::vector<PropertyTile*> validProp = it->second; // valuenya

    selected =command_.getInt(0, completeColourGroup.size());
    if (selected == 0) {
        return;
    }

    StreetTile* chosenPropTile = dynamic_cast<StreetTile*>(validProp.at(selected - 1));

    if (chosenPropTile != nullptr) {
        int price = chosenPropTile->getBuildNextBuildingPrice();
        if (price < p.getBalance()) {
            // anda ga cukup duit
            view_.showMessage("Anda tidak berhasil bangun, silahkan tidur lagi!\n");
            return;
        }

        try {
            chosenPropTile->upgradeBuilding(); // pembandingnya sm properti yg se-colour group aja
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            view_.showMessage("Duit lu ga cukup!\n");
            return;
        }

        // kurangin duitnya
        p.deductMoney(price);
        view_.showMessage("Lalalalallal aku berhasil upgrade!\n");
    }
}

void GameController::processBankruptcyFlow(Player& payer, Player& owner) {
    // Properti berstatus MORTGAGED tidak dapat langsung dijual ke Bank
    // dalam proses likuidasi.
}


// Syarat menggadaikan
// adalah properti berstatus OWNED dan tidak memiliki bangunan
// yang berdiri di atasnya.
void GameController::processMortgage(Player& p) {
    view_.showMessage("Selamat datang di proses pegadain!\n");

    // cari owned property
    std::map<std::string, std::vector<PropertyTile*>> ownedProperty = p.getOwnedPropertiesGroupByColourGroups();

    if (ownedProperty.empty()) {
        view_.showMessage("Ga ada property yang bisa digadai");
        return;
    }
    
    std::vector<PropertyTile*> linearOrdered;

    // buat jadi bentuk vektor juga
    view_.showMessage("Hayo milih dulu\n");
    int count = 0;

    for (auto& keyValue : ownedProperty) {
        std::vector<PropertyTile*>& vec = keyValue.second;
        for (auto& val : vec) {
            linearOrdered.push_back(val);
            count++;
        }
    }

    // =========================================================================
    int selected = command_.getInt(0, count);
    if (selected == 0) {
        view_.showMessage("ga jadi gadai\n");
        return;
    }

    // Jika masih ada bangunan pada colour
    // group yang sama, semua bangunan pada seluruh color group
    // tersebut harus dijual ke Bank terlebih dahulu dengan harga
    // setengah dari harga beli bangunan sebelum properti dapat digadaikan.
    PropertyTile& selectedTile = *linearOrdered.at(selected - 1);

    // maksudnya semua yg ada di colour group itu lah au ah bingung
    std::vector<PropertyTile*> members = ownedProperty[selectedTile.getColourBlock()];

    // cek pada ada bangunan atau engga
    bool foundHasBuilding = false;
    for (size_t i = 0; i < count; i++) {
        if (members.at(i)->hasBuilding()) {
            foundHasBuilding = true;
            break;
        }
    }
    

    // kasus ga ada bangunan
    if (!foundHasBuilding) {
        // langsung sukses
        selectedTile.setToMortgaged();
        // duit player tambahin
        p.addMoney(selectedTile.getSellingPrice());
        view_.showMessage("hore kamu dapat duit bla bla bla\n");
    } else {
        // kasus ada bangunan
        view_.showMessage("di tile ini masih ada bangunannya, tidak dapat digadaikan! jual dulu gih");
        view_.showMessage("Ini dia ni yg masih ada\n");

        bool wantToSellAllBuildings = command_.askWantToSellAllBuildings("mau jual semua gaa\n");
        if (wantToSellAllBuildings) {
            // proses jual semua
            int earnedSum = 0;
            for (size_t i = 0; i < members.size(); i++) {
                earnedSum +=members.at(i)->sellAllBuildings();
                view_.showMessage("Ceklik..\n");
            }
            view_.showMessage("total:///");
            p.addMoney(earnedSum);
        } else {
            view_.showMessage("baiklah bye");
            return;
        }
        // lanjut gadai ga ?
        if (command_.getBool("Mau lanjut gadai ga\n")) {
            selectedTile.setToMortgaged();
            // duit player tambahin
            p.addMoney(selectedTile.getSellingPrice());
            view_.showMessage("hore kamu dapat duit bla bla bla\n");
            return;
        }
    }
}