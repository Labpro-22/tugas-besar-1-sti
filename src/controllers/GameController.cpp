#include "controllers/GameController.hpp"

#include "controllers/AuctionCoordinator.hpp"
#include "controllers/PropertyCoordinator.hpp"
#include "controllers/SkillCardCoordinator.hpp"

#include <utility>

GameController::GameController(std::vector<std::unique_ptr<Player>> players,
    Board& board, Dice& dice, GameViewInterface& view,
    CommandInterface& command, Deck<SkillCard>& specialCardDeck)
    : specialCardDeck_(specialCardDeck), board_(board), dice_(dice),
    view_(view), command_(command), players_(std::move(players)), auction_(players_, view_),
    auctionCoordinator_(std::make_unique<AuctionCoordinator>(players_, view_, command_)),
    skillCardCoordinator_(std::make_unique<SkillCardCoordinator>(specialCardDeck_, view_, command_)),
    propertyCoordinator_(std::make_unique<PropertyCoordinator>(players_, board_, dice_, view_, command_)){}

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

void GameController::processMovement(Player& p, int firstDisplacement) {
	Tile& nextTile = board_.moveToNextTile(p.move(firstDisplacement));

	p.setPosition(nextTile.getTileID());
	p.setLastDiceTotal(firstDisplacement);
	OnLandResult result = nextTile.onLand(p, command_, view_);

	switch (result) {
		case OnLandResult::TriggerAuction:
			break;
		case OnLandResult::TriggerBankruptcyAuction: {
			auction_.runBankruptcyAuction(p, command_);
			break;
		}
		case OnLandResult::TakeChanceCard:
			processTakeChanceCard(p);
			break;
		case OnLandResult::TakeCommunityChest:
			processTakeCommunityChest(p);
			break;
		case OnLandResult::Festival:
			processFestival(p);
			break;
		case OnLandResult::TriggerMoveToJail:
			p.setPosition(board_.getJailPosition());
			view_.showMessage("Kamu dipindahkan ke penjara");
			break;
		case OnLandResult::TriggerTryToPayRent:
			processPayRent(p, nextTile);
			break;
		case OnLandResult::Done:
			break;
		default:
			break;
	}
}

// SKILL CARD RELATED --
void GameController::processPickAndDropSpecialCard(Player& p) {
	skillCardCoordinator_->processPickAndDropSkillCard(p);
}

void GameController::processSpecialCardUse(Player& p, bool& hasUsedSkillCardThisTurn) {
	skillCardCoordinator_->processSkillCardUse(p, hasUsedSkillCardThisTurn);
}
// PROPERTY RELATED ----
void GameController::processFestival(Player& p) {
	propertyCoordinator_->processFestival(p);
}

void GameController::processTakeChanceCard(Player& p) {
	propertyCoordinator_->processTakeChanceCard(p);
}

void GameController::processRedeem(Player& p) {
	propertyCoordinator_->processRedeem(p);
}

void GameController::processPayRent(Player& p, Tile& currentTile) {
	propertyCoordinator_->processPayRent(p, currentTile);
}

void GameController::processBuyBuilding(Player& p) {
	propertyCoordinator_->processBuyBuilding(p);
}

void GameController::processBankruptcyFlow(Player& payer, Player& owner) {
	propertyCoordinator_->processBankruptcyFlow(payer, owner);
}

void GameController::processMortgage(Player& p) {
	propertyCoordinator_->processMortgage(p);
}

// AUCTION RELATED -------
void GameController::transferProperty(Player& from, Player& to, PropertyTile& propertyTile) {
	auctionCoordinator_->transferProperty(from, to, propertyTile);
}

void GameController::processAuction(Player& triggerPlayer, PropertyTile& propertyTile) {
	auctionCoordinator_->processAuction(triggerPlayer, propertyTile);
}

void GameController::processBankruptcyToBank(Player& p) {
	auctionCoordinator_->processBankruptcyToBank(p);
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
    bool hasUsedSkillCardThisTurn = false;

    if (p.isInJail()) {
        if (p.thisTurnAutoFreeFromJail()) {
            view_.showMessage("Sudah 3 turn di penjara. Kamu wajib bayar denda!\n");
            p.deductMoney(jailFine_);
            p.leaveJail();
            processNormalTurn(p, hasUsedSkillCardThisTurn);
        } else {
            processJailTurn(p, hasUsedSkillCardThisTurn);
        }
    } else {
        processNormalTurn(p, hasUsedSkillCardThisTurn);
    }
    
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

	p.resetCountDouble();
	processMovement(p, total);
	return false;
}

void GameController::processNormalTurn(Player& p, bool& hasUsedSkillCardThisTurn) {
    bool hasRolledThisTurn = false;
    bool isTurnActive = true;
    bool canSave = true;

    while (isTurnActive && !p.isBankrupt() && !p.isInJail()) {
        view_.showMessage("\nGiliran " + p.getUsername() + ". Masukkan perintah: ");
        Command cmd = command_.getCommand();
        if (cmd.getType() != CommandType::SIMPAN &&
            cmd.getType() != CommandType::CETAK_PAPAN &&
            cmd.getType() != CommandType::CETAK_AKTA &&
            cmd.getType() != CommandType::CETAK_PROPERTI) {
            canSave = false;
        }
        switch (cmd.getType()) {
            case CommandType::LEMPAR_DADU:
                if (hasRolledThisTurn) {
                    view_.showMessage("Kamu sudah melempar dadu!\n");
                } else {
                    bool isDouble = processRandomDice(p);
                    hasRolledThisTurn = true;
                    
                    if (isDouble && !p.isInJail() && !p.isBankrupt()) {
                        view_.showMessage("DOUBLE! Kamu dapat kesempatan lempar lagi.\n");
                        hasRolledThisTurn = false; 
                    } else {
                        isTurnActive = false; 
                    }
                }
                break;
            case CommandType::ATUR_DADU: {
                if (cmd.getArgCount() < 2) {
                    view_.showMessage("Format salah. Gunakan: ATUR_DADU X Y\n");
                    break;
                }
                if (hasRolledThisTurn) {
                    view_.showMessage("Kamu sudah melempar dadu!\n");
                } else {
                    int x = cmd.getArg(0);
                    int y = cmd.getArg(1);
                    bool isDouble = processCustomDice(p, x, y);
                    hasRolledThisTurn = true;

                    if (isDouble && !p.isInJail() && !p.isBankrupt()) {
                        view_.showMessage("DOUBLE! Kamu dapat kesempatan lempar lagi.\n");
                        hasRolledThisTurn = false;
                    } else {
                        isTurnActive = false;
                    }
                }
                break;
            }
            case CommandType::GUNAKAN_KEMAMPUAN:
                if (!hasRolledThisTurn) {
                    processSpecialCardUse(p, hasUsedSkillCardThisTurn);
                } else {
                    view_.showMessage("Kemampuan hanya bisa digunakan sebelum lempar dadu.\n");
                }
                break;
            case CommandType::SIMPAN:
                if (canSave) {
                    // processSave();
                    view_.showMessage("Permainan berhasil disimpan.\n");
                } else {
                    view_.showMessage("Tidak bisa SIMPAN. Kamu sudah melakukan aksi/pergerakan.\n");
                }
                break;
            case CommandType::CETAK_PAPAN: view_.cetakPapan(); break;
            case CommandType::CETAK_AKTA: view_.cetakAkta(); break;
            case CommandType::CETAK_PROPERTI: view_.cetakProperti(); break;
            case CommandType::GADAI: processMortgage(p); break;
            case CommandType::TEBUS: processRedeem(p); break;
            case CommandType::BANGUN: processBuyBuilding(p); break;
            default:
                view_.showMessage("Perintah tidak dikenal.\n");
                break;
        }
    }
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
			p.deductMoney(jailFine_);
			p.leaveJail();
			processNormalTurn(p, hasUsedSkillCardThisTurn);
			break;
		default:
			break;
	}
}