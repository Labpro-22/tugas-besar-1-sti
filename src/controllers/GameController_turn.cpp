#include "controllers/GameController.hpp"

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

	p.resetCountDouble();
	processMovement(p, total);
	return false;
}

void GameController::processNormalTurn(Player& p, bool& hasUsedSkillCardThisTurn) {
	bool hasRolledFirstTime = false;
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
			case CommandType::ATUR_DADU: {
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
			}
			case CommandType::CETAK_AKTA:
				view_.cetakAkta();
				break;
			case CommandType::CETAK_PROPERTI:
				view_.cetakProperti();
				break;
			case CommandType::GADAI:
				processMortgage(p);
				break;
			case CommandType::TEBUS:
				processRedeem(p);
				break;
			case CommandType::BANGUN:
				processBuyBuilding(p);
				break;
			case CommandType::SIMPAN:
				break;
			case CommandType::MUAT:
				break;
			case CommandType::CETAK_LOG:
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
