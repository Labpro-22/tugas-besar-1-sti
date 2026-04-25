#include "controllers/GameController.hpp"

void GameController::transferProperty(Player& from, Player& to, PropertyTile& propertyTile) {
	PropertyStatus oldStatus = propertyTile.getPropertyStatus();

	from.removeProperty(&propertyTile);
	to.addProperty(&propertyTile);

	propertyTile.setPropertyStatus(oldStatus);
}

void GameController::processAuction(Player& triggerPlayer, PropertyTile& propertyTile) {
	view_.showMessage("Properti " + propertyTile.getTileName() + " (" + propertyTile.getLetterCode() + ") akan dilelang!\n");

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

		p.removeProperty(property);
		property->resetAfterBankruptcyToBank();

		view_.showMessage("\n-> Lelang: " + property->getTileName() +
			" (" + property->getLetterCode() + ")\n");

		processAuction(p, *property);
	}

	p.setStatus(Player::PlayerStatus::BANKRUPT);
	view_.showMessage(p.getUsername() + " telah keluar dari permainan.\n");
}
