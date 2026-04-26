#include "controllers/GameController.hpp"

#include "controllers/AuctionCoordinator.hpp"
#include "controllers/PropertyCoordinator.hpp"
#include "controllers/SkillCardCoordinator.hpp"
#include "controllers/GameConfig.hpp"
#include "models/tile/action_tile/special_tile/GoTile.hpp"
#include "models/tile/property_tile/StreetTile.hpp"
#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/tile/property_tile/UtilityTile.hpp"

#include <map>
#include <iomanip>
#include <sstream>
#include <utility>

GameController::GameController(std::vector<std::unique_ptr<Player>> players,
    Board& board, Dice& dice, GameViewInterface& view,
    CommandInterface& command, Deck<SkillCard>& specialCardDeck)
    : specialCardDeck_(specialCardDeck), board_(board), dice_(dice),
    view_(view), command_(command), players_(std::move(players)), auction_(players_, view_),
    auctionCoordinator_(std::make_unique<AuctionCoordinator>(players_, view_, command_)),
    skillCardCoordinator_(std::make_unique<SkillCardCoordinator>(specialCardDeck_, view_, command_)),
    propertyCoordinator_(std::make_unique<PropertyCoordinator>(players_, board_, dice_, view_, command_)) {}

GameController::~GameController() = default;

void GameController::setCurrentTurn(int latestTurn) {
    currentTurn_ = latestTurn;
}

// TERKAIT LOGIC GAME SECARA UMUM =========================================================================================================
void GameController::playGame(int latestTurn, int maxTurn) {
    currentTurn_ = latestTurn;
    transactionLog_.clear();
    while ((currentTurn_ <= maxTurn || maxTurn == -1) && !hasSoleWinner()) {
        for (auto& player : players_) {
            if (!player->isBankrupt()) {
                processTurn(*player);
            }
        }
        currentTurn_++;
    }
    decideWinner();
}

void GameController::processMovement(Player& p, int firstDisplacement) {
    Tile& nextTile = board_.moveToNextTile(p.move(firstDisplacement));

    int oldIndex = p.getPosition();
    int distance = oldIndex + firstDisplacement;
    bool passedStart = distance >= board_.getSize();

    if (passedStart) {
        int nextIndex = board_.getTileIndexByCode(nextTile.getLetterCode());
        if (nextIndex != board_.getStartPosition()) {
            try {
                Tile& goTileRaw = board_.getTileByCode("GO");
                GoTile* goTile = dynamic_cast<GoTile*>(&goTileRaw);
                if (goTile != nullptr) {
                    p.addMoney(goTile->getSalary());
                    view_.showMessage("Melewati GO: menerima salary sebesar " + std::to_string(goTile->getSalary()) + "\n");
                    addTransactionLog(p.getUsername(), "GO", "Melewati START dan menerima M" + std::to_string(goTile->getSalary()));
                }
            } catch (...) {
            }
        }
    }

    p.setPosition(board_.getTileIndexByCode(nextTile.getLetterCode()));
    addTransactionLog(p.getUsername(), "PINDAH", "Mendarat di " + nextTile.getTileName() + " (" + nextTile.getLetterCode() + ")");
    OnLandResult result = nextTile.onLand(p, command_, view_);

        switch (result) {
            case OnLandResult::TriggerAuction: {
                PropertyTile* property = dynamic_cast<PropertyTile*>(&nextTile);
                if (property != nullptr) {
                    auctionCoordinator_->processAuction(p, *property);
                } else {
                    view_.showMessage("Auction trigger on non-property tile.\n");
                }
                break;
            }
		case OnLandResult::TriggerBankruptcyAuction: {
			auction_.runBankruptcyAuction(p, command_);
			break;
		}
		case OnLandResult::TakeChanceCard:
			propertyCoordinator_->processTakeChanceCard(p);
			break;
		case OnLandResult::TakeCommunityChest:
            try {
			    propertyCoordinator_->processTakeCommunityChest(p);
            }
            catch(const CardPaymentFailedException& e) {
                view_.showMessage(e.getErrorMessage());
                // proses kebangkrutannya
                auction_.processBankruptcyToBank(p);
            }
			break;
		case OnLandResult::Festival:
            propertyCoordinator_->processFestival(p);
			break;
		case OnLandResult::TriggerMoveToJail:
            p.setPosition(board_.getJailPosition());
            p.setStatus(Player::PlayerStatus::JAILED);
            p.resetJailTurn();
            view_.showMessage("Kamu dipindahkan ke penjara\n");
			break;
		case OnLandResult::TriggerTryToPayRent:
			propertyCoordinator_->processPayRent(p, nextTile);
			break;
		case OnLandResult::Done:
			break;
		default:
			break;
	}
}

void GameController::handleSkillCardUsage(Player& p, SkillCard& card) {
    if (auto* moveCard = dynamic_cast<MoveCard*>(&card)) {
        executeMoveCard(p, *moveCard);
    } 
    else if (dynamic_cast<TeleportCard*>(&card)) {
        executeTeleportCard(p);
    } 
    else if (dynamic_cast<LassoCard*>(&card)) {
        executeLassoCard(p);
    }
}

// LOGIKA MOVE CARD
void GameController::executeMoveCard(Player& p, MoveCard& card) {
    int steps = card.getSteps();
    view_.showMessage(p.getUsername() + " menggunakan Move Card untuk maju " + std::to_string(steps) + " langkah.");
    processMovement(p, steps);
}

// LOGIKA TELEPORT CARD
void GameController::executeTeleportCard(Player& p) {
    int maxIdx = board_.getSize() - 1;
    int target;
    view_.showMessage("Masukkan ID petak tujuan (0-" + std::to_string(maxIdx) + "): ");
    
    std::cin >> target; 

    if (target >= 0 && target <= maxIdx) {
        p.setPosition(target);
        view_.showMessage(p.getUsername() + " teleport ke petak " + std::to_string(target));
        
        board_.getCurrentTile(target).onLand(p, command_, view_);
    } else {
        view_.showMessage("ID Petak tidak valid.");
    }
}

// LOGIKA LASSO CARD
void GameController::executeLassoCard(Player& owner) {
    int ownerPos = owner.getPosition();
    Player* victim = nullptr;
    int minDistance = 9999;
    int boardSize = board_.getSize();

    for (Player* candidate : owner.getAllPlayers()) {
        if (candidate == nullptr || candidate == &owner || candidate->isBankrupt()) continue;

        int dist = (candidate->getPosition() - ownerPos + boardSize) % boardSize;
        
        if (dist > 0 && dist < minDistance) {
            minDistance = dist;
            victim = candidate;
        }
    }

    if (victim) {
        view_.showMessage(owner.getUsername() + " menarik " + victim->getUsername() + " ke petak " + std::to_string(ownerPos));
        victim->setPosition(ownerPos);
        board_.getCurrentTile(ownerPos).onLand(*victim, command_, view_);
    } else {
        view_.showMessage("Tidak ada pemain di depan yang bisa ditarik.");
    }
}

bool GameController::hasSoleWinner() const{
    size_t countNotBankrupt = 0;
    for (auto& player : players_) {
        if (!player->isBankrupt()) {
            countNotBankrupt++;
        }
    }
    return countNotBankrupt == 1;
}


void GameController::decideWinner() const {
    std::vector<Player*> candidates;
    for (const auto& p : players_) {
        if (!p->isBankrupt()) candidates.push_back(p.get());
    }

    if (candidates.empty()) {
        view_.showMessage("Semua pemain bangkrut.\n");
        return;
    }

    Player* topPlayer = candidates[0];
    for (size_t i = 1; i < candidates.size(); i++) {
        if (*candidates[i] > *topPlayer) {
            topPlayer = candidates[i];
        }
    }

    std::vector<Player*> winners;
    for (auto* p : candidates) {
        if (*p == *topPlayer) { // Memanggil operator == yang kita buat
            winners.push_back(p);
        }
    }

    // 3. Tampilkan hasil
    if (winners.size() == 1) {
        view_.showMessage("Pemenang: " + winners[0]->getUsername() + "\n");
        view_.showMessage("Uang: M" + std::to_string(winners[0]->getBalance()) + "\n");
        view_.showMessage("Jumlah properti: " + std::to_string(winners[0]->getPropertyCount()) + "\n");
        view_.showMessage("Jumlah kartu: " + std::to_string(winners[0]->getCardCount()) + "\n");
    } else {
        view_.showMessage("Permainan berakhir seri. Para pemenang:\n");
        for (Player* player : winners) {
            view_.showMessage("- " + player->getUsername() +
                            " | Uang: M" + std::to_string(player->getBalance()) +
                            " | Properti: " + std::to_string(player->getPropertyCount()) +
                            " | Kartu: " + std::to_string(player->getCardCount()) +
                            "\n");
        }
    }
}

void GameController::processTurn(Player& p) {
    view_.showMessage("\n--- Giliran " + p.getUsername() + " ---");
    addTransactionLog(p.getUsername(), "TURN", "Mulai giliran");
    bool hasUsedSkillCardThisTurn = false;
    bool hasRolledDiceThisTurn = false;
    bool canRollDice = true;
    bool isTurnActive = true;

    // Setiap awal giliran, pemain mengambil 1 kartu kemampuan dari deck.
    skillCardCoordinator_->processPickAndDropSkillCard(p);

    while (isTurnActive && !p.isBankrupt()) {
        if (p.isInJail()) {
            isTurnActive = processJailTurn(p, hasUsedSkillCardThisTurn, hasRolledDiceThisTurn, canRollDice);
        } else {
            isTurnActive = processNormalTurn(p, hasUsedSkillCardThisTurn, hasRolledDiceThisTurn, canRollDice);
        }
    }
    
    p.decreaseShieldCardTurn();
}

bool GameController::processRandomDice(Player& p){
	view_.showMessage("Mengocok dadu...\n");
	dice_.roll();
	return resolveDiceResult(p, dice_.getDie1(), dice_.getDie2());
}

bool GameController::processCustomDice(Player& p, int x, int y){
	view_.showMessage("Dadu diatur secara manual.\n");
	dice_.rollSettingan(x, y);
	return resolveDiceResult(p, dice_.getDie1(), dice_.getDie2());
}

bool GameController::resolveDiceResult(Player& p, int d1, int d2){
	int total = d1 + d2;
    Tile& predictedTile = board_.moveToNextTile(p.move(total));
    addTransactionLog(
        p.getUsername(),
        "DADU",
        "Lempar: " + std::to_string(d1) + "+" + std::to_string(d2) + "=" + std::to_string(total) +
        " -> mendarat di " + predictedTile.getTileName() + " (" + predictedTile.getLetterCode() + ")"
    );
	view_.showMessage("Hasil: " + std::to_string(d1) + " + " + std::to_string(d2) + " = " + std::to_string(total) + "\n");
	view_.showMessage("Memajukan Bidak " + p.getUsername() + " sebanyak " + std::to_string(total) +" petakk...\n");
	if (d1 == d2) {
		p.incrementDoubleCount();
        addTransactionLog(p.getUsername(), "DOUBLE", "Mendapat giliran tambahan");
		if (!p.notViolatingDoubleRollCount()) {
			view_.showMessage("Triple double! Masuk penjara.\n");
            addTransactionLog(p.getUsername(), "DOUBLE", "Triple double -> masuk penjara");
			p.setStatus(Player::PlayerStatus::JAILED);
			p.resetJailTurn();
			p.setPosition(board_.getJailPosition());
			return false;
		}
		processMovement(p, total);
		return true;
	}

	p.resetCountDouble();
	processMovement(p, total);
	return false;
}

bool GameController::processNormalTurn(Player& p, bool& hasUsedSkillCardThisTurn, bool& hasRolledDiceThisTurn, bool& canRollDice) {
    view_.showMessage("\nGiliran " + p.getUsername() + ". Masukkan perintah: ");
    Command cmd = command_.getCommand();
    while (cmd.getType() == CommandType::INVALID) {
        view_.showMessage("Perintah tidak valid. Coba lagi: ");
        cmd = command_.getCommand();
    }
    

    switch (cmd.getType()) {
        case CommandType::LEMPAR_DADU: {
            if (!canRollDice) {
                view_.showMessage("Kamu sudah melempar dadu. Gunakan END_TURN untuk mengakhiri giliran.\n");
                return true;
            }

            bool isDouble = processRandomDice(p);
            hasRolledDiceThisTurn = true;

            if (p.isBankrupt()) {
                return false;
            }

            if (p.isInJail()) {
                return true;
            }

            if (isDouble) {
                view_.showMessage("DOUBLE! Kamu boleh melempar dadu lagi kapan saja sebelum END_TURN.\n");
                canRollDice = true;
            } else {
                canRollDice = false;
            }

            return true;
        }

        case CommandType::ATUR_DADU: {
            if (!canRollDice) {
                view_.showMessage("Kamu sudah melempar dadu. Gunakan END_TURN untuk mengakhiri giliran.\n");
                return true;
            }

            if (cmd.getArgCount() < 2) {
                view_.showMessage("Format salah. Gunakan: ATUR_DADU X Y\n");
                return true;
            }

            int x = cmd.getArg(0);
            int y = cmd.getArg(1);

            bool isDouble = processCustomDice(p, x, y);
            hasRolledDiceThisTurn = true;

            if (p.isBankrupt()) {
                return false;
            }

            if (p.isInJail()) {
                return true;
            }

            if (isDouble) {
                view_.showMessage("DOUBLE! Kamu boleh melempar dadu lagi kapan saja sebelum END_TURN.\n");
                canRollDice = true;
            } else {
                canRollDice = false;
            }

            return true;
        }

        case CommandType::GUNAKAN_KEMAMPUAN: {
            if (hasRolledDiceThisTurn) {
                view_.showMessage("Kemampuan hanya bisa digunakan sebelum lempar dadu pertama pada turn ini.\n");
                return true;
            }

            addTransactionLog(p.getUsername(), "KARTU", "Menggunakan kartu kemampuan");
            std::unique_ptr<SkillCard> kartu = skillCardCoordinator_->processSkillCardUse(p, hasUsedSkillCardThisTurn);
            if (kartu) {
                addTransactionLog(p.getUsername(), "KARTU", "Menggunakan kartu: " + kartu->getName());
                this->handleSkillCardUsage(p, *kartu);
                specialCardDeck_.pushToDiscard(std::move(kartu));
            }
            return true;
        }

        case CommandType::GADAI: {
            propertyCoordinator_->processMortgage(p);
            return true;
        }

        case CommandType::TEBUS: {
            propertyCoordinator_->processRedeem(p);
            return true;
        }

        case CommandType::BANGUN: {
            propertyCoordinator_->processBuyBuilding(p);
            return true;
        }

        case CommandType::CETAK_PAPAN: {
            view_.cetakPapan(p, currentTurn_);
            return true;
        }

        case CommandType::CETAK_AKTA: {
            view_.showMessage("\nMasukkan kode petak: ");
            std::string kodePetak;
            // TODO: inputnya enaknya gimana yah???
            std::cin >> kodePetak;

            std::cin.ignore(10000, '\n');

            for (char &c : kodePetak) {
                c = toupper(c);
            }

            view_.cetakAkta(kodePetak);

            return true;
        }

        case CommandType::CETAK_PROPERTI: {
            view_.cetakProperti(&p);
            return true;
        }

        case CommandType::INVENTORY: {
            showInventory(p);
            return true;
        }

        case CommandType::POSITION: {
            showPosition(p);
            return true;
        }

        case CommandType::CETAK_LOG: {
            if (cmd.getArgCount() == 0) {
                printTransactionLog();
            } else {
                int n = cmd.getArg(0);
                if (n <= 0) {
                    view_.showMessage("Argumen CETAK_LOG harus > 0. Contoh: CETAK_LOG 5\n");
                } else {
                    printTransactionLog(n);
                }
            }
            return true;
        }

        case CommandType::SIMPAN: {
            if (hasRolledDiceThisTurn) {
                view_.showMessage("Tidak bisa SIMPAN setelah melempar dadu.\n");
            } else {
                view_.showMessage("Permainan berhasil disimpan.\n");
            }
            return true;
        }

        case CommandType::END_COMMAND: {
            processEndTurn(p);
            return false;
        }

        default: {
            view_.showMessage("Perintah tidak dikenal.\n");
            return true;
        }
    }
}

bool GameController::processJailTurn(Player& p, bool& hasUsedSkillCardThisTurn, bool& hasRolledDiceThisTurn, bool& canRollDice) {
    view_.showMessage("\nAnda sedang berada di penjara.\n");
    view_.showMessage("Command valid: BAYAR_DENDA, LEMPAR_DADU, ATUR_DADU X Y, GUNAKAN_KEMAMPUAN, INVENTORY, POSITION, END_COMMAND\n");

    Command cmd = command_.getCommand();
    while (cmd.getType() == CommandType::INVALID) {
        view_.showMessage("Perintah tidak valid. Coba lagi: ");
        cmd = command_.getCommand();
    }

    switch (cmd.getType()) {
        case CommandType::BAYAR_DENDA: {
            bool wajibBayar = p.thisTurnAutoFreeFromJail();

            if (p.getBalance() < GameConfig::getJailFine()) {
                if (wajibBayar) {
                    view_.showMessage("Kamu tidak mampu membayar denda wajib.\n");
                    auctionCoordinator_->processBankruptcyToBank(p);
                    return false;
                }

                view_.showMessage("Uang tidak cukup. Kamu tetap berada di penjara.\n");
                return true;
            }

            p.deductMoney(GameConfig::getJailFine());
            p.leaveJail();
            p.resetJailTurn();

            view_.showMessage("Denda dibayar. Kamu keluar dari penjara.\n");
            return true;
        }

        case CommandType::LEMPAR_DADU: {
            if (!canRollDice) {
                view_.showMessage("Kamu sudah mencoba dadu pada turn penjara ini.\n");
                return true;
            }

            dice_.roll();

            int d1 = dice_.getDie1();
            int d2 = dice_.getDie2();
            int total = d1 + d2;

            view_.showMessage("Hasil: " + std::to_string(d1) + " + " + std::to_string(d2) + " = " + std::to_string(total) + "\n");

            hasRolledDiceThisTurn = true;
            canRollDice = false;

            if (d1 == d2) {
                view_.showMessage("DOUBLE! Kamu keluar dari penjara dan bergerak.\n");

                p.leaveJail();
                p.resetJailTurn();
                p.resetCountDouble();

                processMovement(p, total);
                return true;
            }

            if (p.thisTurnAutoFreeFromJail()) {
                view_.showMessage("Percobaan terakhir gagal. Kamu wajib membayar denda.\n");

                if (p.getBalance() < GameConfig::getJailFine()) {
                    auctionCoordinator_->processBankruptcyToBank(p);
                    return false;
                }

                p.deductMoney(GameConfig::getJailFine());
                p.leaveJail();
                p.resetJailTurn();

                view_.showMessage("Denda dibayar. Kamu keluar dari penjara.\n");
                return true;
            }

            view_.showMessage("Tidak double. Kamu tetap di penjara.\n");
            return true;
        }

        case CommandType::ATUR_DADU: {
            if (!canRollDice) {
                view_.showMessage("Kamu sudah mencoba dadu pada turn penjara ini.\n");
                return true;
            }

            if (cmd.getArgCount() < 2) {
                view_.showMessage("Format salah. Gunakan: ATUR_DADU X Y\n");
                return true;
            }

            int x = cmd.getArg(0);
            int y = cmd.getArg(1);

            if (x < 1 || x > 6 || y < 1 || y > 6) {
                view_.showMessage("Nilai dadu harus antara 1 sampai 6.\n");
                return true;
            }

            dice_.rollSettingan(x, y);

            int d1 = dice_.getDie1();
            int d2 = dice_.getDie2();
            int total = d1 + d2;

            view_.showMessage("Hasil: " + std::to_string(d1) + " + " + std::to_string(d2) + " = " + std::to_string(total) + "\n");

            hasRolledDiceThisTurn = true;
            canRollDice = false;

            if (d1 == d2) {
                view_.showMessage("DOUBLE! Kamu keluar dari penjara dan bergerak.\n");

                p.leaveJail();
                p.resetJailTurn();
                p.resetCountDouble();

                processMovement(p, total);
                return true;
            }

            if (p.thisTurnAutoFreeFromJail()) {
                view_.showMessage("Percobaan terakhir gagal. Kamu wajib membayar denda.\n");

                if (p.getBalance() < GameConfig::getJailFine()) {
                    auctionCoordinator_->processBankruptcyToBank(p);
                    return false;
                }

                p.deductMoney(GameConfig::getJailFine());
                p.leaveJail();
                p.resetJailTurn();

                view_.showMessage("Denda dibayar. Kamu keluar dari penjara.\n");
                return true;
            }

            view_.showMessage("Tidak double. Kamu tetap di penjara.\n");
            return true;
        }

        case CommandType::GUNAKAN_KEMAMPUAN: {
            if (hasRolledDiceThisTurn) {
                view_.showMessage("Kemampuan hanya bisa digunakan sebelum mencoba dadu.\n");
                return true;
            }

            std::unique_ptr<SkillCard> kartu = skillCardCoordinator_->processSkillCardUse(p, hasUsedSkillCardThisTurn);
            if (kartu) {
                this->handleSkillCardUsage(p, *kartu);
                specialCardDeck_.pushToDiscard(std::move(kartu));
            }
            return true;
        }

        case CommandType::INVENTORY: {
            showInventory(p);
            return true;
        }

        case CommandType::POSITION: {
            showPosition(p);
            return true;
        }

        case CommandType::CETAK_LOG: {
            if (cmd.getArgCount() == 0) {
                printTransactionLog();
            } else {
                int n = cmd.getArg(0);
                if (n <= 0) {
                    view_.showMessage("Argumen CETAK_LOG harus > 0. Contoh: CETAK_LOG 5\n");
                } else {
                    printTransactionLog(n);
                }
            }
            return true;
        }

        case CommandType::END_COMMAND: {
            if (p.isInJail()) {
                p.incrementJailTurn();
            }

            processEndTurn(p);
            return false;
        }

        default: {
            view_.showMessage("Perintah tidak valid saat berada di penjara.\n");
            return true;
        }
    }
}

void GameController::showPosition(Player& p) {
    Tile& tile = board_.getCurrentTile(p.getPosition());

    view_.showMessage("\n=== POSISI PEMAIN ===\n");
    view_.showMessage("Pemain : " + p.getUsername() + "\n");
    view_.showMessage("Index  : " + std::to_string(p.getPosition()) + "\n");
    view_.showMessage("Kode   : " + tile.getLetterCode() + "\n");
    view_.showMessage("Nama   : " + tile.getTileName() + "\n");
    view_.showMessage("Warna  : " + tile.getColourBlock() + "\n");

    PropertyTile* property = dynamic_cast<PropertyTile*>(&tile);
    if (property != nullptr) {
        view_.showMessage("Owner  : " + property->getOwnerUsername() + "\n");

        if (property->getPropertyStatus() == BANK) {
            view_.showMessage("Status : BANK\n");
        } else if (property->getPropertyStatus() == OWNED) {
            view_.showMessage("Status : OWNED\n");
        } else if (property->getPropertyStatus() == MORTGAGED) {
            view_.showMessage("Status : MORTGAGED\n");
        }
    }
}

void GameController::showInventory(Player& p) {
    view_.showMessage("\n================ INVENTORY ================\n");
    view_.showMessage("Pemain  : " + p.getUsername() + "\n");
    view_.showMessage("Balance : M" + std::to_string(p.getBalance()) + "\n");

    view_.showMessage("\n=== PROPERTY ===\n");

    std::vector<PropertyTile*> properties = p.getProperties();

    if (properties.empty()) {
        view_.showMessage("Tidak ada property.\n");
    } else {
        view_.showMessage("Kode | Nama | Tipe | Status | Bangunan | Harga Beli | Gadai | Upgrade | Rent\n");
        view_.showMessage("-------------------------------------------------------------------------------\n");

        for (size_t i = 0; i < properties.size(); i++) {
            PropertyTile* property = properties[i];

            if (property == nullptr) {
                continue;
            }

            std::string type = "Property";
            std::string upgrade = "-";
            std::string rent = "-";

            StreetTile* street = dynamic_cast<StreetTile*>(property);
            RailRoadTile* rail = dynamic_cast<RailRoadTile*>(property);
            UtilityTile* utility = dynamic_cast<UtilityTile*>(property);

            if (street != nullptr) {
                type = "Street";

                std::map<int, int> buildPrice = street->getBuildPrice();
                int nextLevel = street->getLevel() + 1;

                if (street->getLevel() >= 5) {
                    upgrade = "MAX";
                } else {
                    if (buildPrice.count(nextLevel) > 0) {
                        upgrade = "M" + std::to_string(buildPrice[nextLevel]);
                    } else {
                        upgrade = "-";
                    }
                }

                bool completedColourGroup = board_.isCompletedColourGroup(
                    p.getUsername(),
                    property->getColourBlock()
                );

                rent = "M" + std::to_string(street->calculateRentPrice(completedColourGroup));
            } else if (rail != nullptr) {
                type = "RailRoad";

                int countRail = p.countRailroad();
                rent = "M" + std::to_string(rail->calculateRentPrice(countRail));
            } else if (utility != nullptr) {
                type = "Utility";

                int diceTotal = dice_.getRollResult();
                if (diceTotal <= 0) {
                    rent = "Total dadu x faktor utility";
                } else {
                    int countUtility = p.countUtilities();
                    rent = "M" + std::to_string(utility->calculateRentPrice(countUtility, diceTotal));
                }
            }

            view_.showMessage(property->getLetterCode() + " | " + property->getTileName() + " | " + type + " | " + property->propertyStatusToText() + " | " + property->buildingLevelToText() + " | " + "M" + std::to_string(property->getPurchasePrice()) + " | " + "M" + std::to_string(property->getMortgageValue()) + " | " + upgrade + " | " + rent + "\n");
        }
    }

    view_.showMessage("\n=== SKILL CARD ===\n");

    std::vector<SkillCard*> cards = p.getSkillCards();

    if (cards.empty()) {
        view_.showMessage("Tidak ada kartu.\n");
    } else {
        for (size_t i = 0; i < cards.size(); i++) {
            if (cards[i] == nullptr) {
                continue;
            }
            view_.showMessage(std::to_string(i + 1) + ". " + cards[i]->getName() + " - " + cards[i]->getDescription() + "\n");
        }
    }
    view_.showMessage("===========================================\n");
}

void GameController::processEndTurn(Player& p) {
    p.resetCountDouble();
    view_.showMessage("Turn " + p.getUsername() + " selesai.\n");
    addTransactionLog(p.getUsername(), "TURN", "Selesai giliran");
}

void GameController::addTransactionLog(const std::string& username, const std::string& action, const std::string& detail) {
    std::ostringstream oss;
    oss << "[Turn " << currentTurn_ << "] "
        << username << " | "
        << std::left << std::setw(8) << action << " | "
        << detail;
    transactionLog_.push_back(oss.str());
}

void GameController::printTransactionLog(int lastN) {
    if (lastN > 0) {
        view_.showMessage("\n=== Log Transaksi (" + std::to_string(lastN) + " Terakhir) ===\n\n");
    } else {
        view_.showMessage("\n=== Log Transaksi Penuh ===\n\n");
    }

    if (transactionLog_.empty()) {
        view_.showMessage("Belum ada transaksi tercatat.\n\n");
        return;
    }

    size_t startIdx = 0;
    if (lastN > 0 && static_cast<size_t>(lastN) < transactionLog_.size()) {
        startIdx = transactionLog_.size() - static_cast<size_t>(lastN);
    }

    for (size_t i = startIdx; i < transactionLog_.size(); ++i) {
        view_.showMessage(transactionLog_[i] + "\n");
    }
    view_.showMessage("\n");
}

