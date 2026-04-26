#include "controllers/GameController.hpp"

#include "controllers/AuctionCoordinator.hpp"
#include "controllers/PropertyCoordinator.hpp"
#include "controllers/SkillCardCoordinator.hpp"
#include "models/tile/action_tile/special_tile/GoTile.hpp"

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
                }
            } catch (...) {
            }
        }
    }

    p.setPosition(board_.getTileIndexByCode(nextTile.getLetterCode()));
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
			propertyCoordinator_->processTakeCommunityChest(p);
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
    bool hasUsedSkillCardThisTurn = false;

    // Setiap awal giliran, pemain mengambil 1 kartu kemampuan dari deck.
    skillCardCoordinator_->processPickAndDropSkillCard(p);

    if (p.isInJail()) {
        if (p.thisTurnAutoFreeFromJail()) {
            view_.showMessage("Sudah 3 turn di penjara. Kamu wajib bayar denda!\n");
            p.deductMoney(jailFine_);
            p.leaveJail();
            processNormalTurn(p, hasUsedSkillCardThisTurn);
        } else {
            processJailTurn(p, hasUsedSkillCardThisTurn);
            p.incrementJailTurn();
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
                    skillCardCoordinator_->processSkillCardUse(p, hasUsedSkillCardThisTurn);
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
            case CommandType::GADAI: propertyCoordinator_->processMortgage(p); break;
            case CommandType::TEBUS: propertyCoordinator_->processRedeem(p); break;
            case CommandType::BANGUN: propertyCoordinator_->processBuyBuilding(p); break;
            default:
                view_.showMessage("Perintah tidak dikenal.\n");
                break;
        }
    }
}

void GameController::processJailTurn(Player& p, bool& hasUsedSkillCardThisTurn) {
	bool isJailTurnActive = true;
    bool hasTriedDice = false;

	view_.showMessage("Anda sedang berada di penjara!\n");
    view_.showMessage("Pilih aksi: BAYAR_DENDA, LEMPAR_DADU, ATUR_DADU X Y, GUNAKAN_KEMAMPUAN\n");
	while (isJailTurnActive && p.isInJail() && !p.isBankrupt()) {
        view_.showMessage("\nMasukkan perintah: ");
        Command cmd = command_.getCommand();
        switch (cmd.getType()) {
            case CommandType::BAYAR_DENDA: {
                bool wajibBayar = p.thisTurnAutoFreeFromJail();

                if (p.getBalance() < jailFine_) {
                    if (wajibBayar) {
                        view_.showMessage("Kamu tidak mampu membayar denda wajib!\n");
                        auctionCoordinator_->processBankruptcyToBank(p);
                        return;
                    }

                    view_.showMessage("Uang tidak cukup. Kamu tetap di penjara.\n");
                    return;
                }

                p.deductMoney(jailFine_);
                p.leaveJail();
                p.resetJailTurn();

                view_.showMessage("Denda dibayar. Kamu keluar dari penjara.\n");
                processNormalTurn(p, hasUsedSkillCardThisTurn);
                return;
            }
            case CommandType::LEMPAR_DADU: {
                if (hasTriedDice) {
                    view_.showMessage("Kamu sudah mencoba melempar dadu untuk keluar dari penjara.\n");
                    break;
                }

                view_.showMessage("Mencoba keluar dari penjara dengan lempar dadu...\n");
                dice_.roll();

                int d1 = dice_.getDie1();
                int d2 = dice_.getDie2();
                int total = d1 + d2;

                view_.showMessage("Hasil: " + std::to_string(d1) + " + " + std::to_string(d2) + " = " + std::to_string(total) + "\n");

                hasTriedDice = true;

                if (d1 == d2) {
                    view_.showMessage("DOUBLE! Kamu keluar dari penjara dan bergerak.\n");

                    p.leaveJail();
                    p.resetJailTurn();
                    p.resetCountDouble();

                    processMovement(p, total);
                } else {
                    view_.showMessage("Tidak double. Kamu tetap di penjara dan tidak bergerak.\n");
                }

                isJailTurnActive = false;
                break;
            }
            case CommandType::ATUR_DADU: {
                if (hasTriedDice) {
                    view_.showMessage("Kamu sudah mencoba melempar dadu untuk keluar dari penjara.\n");
                    break;
                }

                if (cmd.getArgCount() < 2) {
                    view_.showMessage("Format salah. Gunakan: ATUR_DADU X Y\n");
                    break;
                }

                int x = cmd.getArg(0);
                int y = cmd.getArg(1);

                if (x < 1 || x > 6 || y < 1 || y > 6) {
                    view_.showMessage("Nilai dadu harus antara 1 sampai 6.\n");
                    break;
                }

                dice_.rollSettingan(x, y);

                int d1 = dice_.getDie1();
                int d2 = dice_.getDie2();
                int total = d1 + d2;

                view_.showMessage("Dadu diatur secara manual.\n");
                view_.showMessage("Hasil: " + std::to_string(d1) + " + " +
                                  std::to_string(d2) + " = " + std::to_string(total) + "\n");

                hasTriedDice = true;

                if (d1 == d2) {
                    view_.showMessage("DOUBLE! Kamu keluar dari penjara dan bergerak.\n");

                    p.leaveJail();
                    p.resetJailTurn();
                    p.resetCountDouble();

                    processMovement(p, total);
                } else {
                    view_.showMessage("Tidak double. Kamu tetap di penjara dan tidak bergerak.\n");
                }

                isJailTurnActive = false;
                break;
            }
            case CommandType::GUNAKAN_KEMAMPUAN:
                skillCardCoordinator_->processSkillCardUse(p, hasUsedSkillCardThisTurn);
                break;

            default:
                view_.showMessage("Perintah tidak valid saat berada di penjara.\n");
                break;
        }
    }
}