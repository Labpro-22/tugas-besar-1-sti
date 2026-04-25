#include "controllers/GameController.hpp"

#include <iostream>
#include <utility>

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
		std::cerr << e.what() << '\n';
		view_.showMessage("Silahkan pilih yang mau dibuang terlebih dahulu\n");

		int selected = command_.getInt(1, p.getSkillCardCount() + 1);
		if (selected == p.getSkillCardCount() + 1) {
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
	view_.showMessage("Kartu " + usedCard->getName() + " telah dipakai.\n");

	specialCardDeck_.pushToDiscard(std::move(usedCard));
	hasUsedSkillCardThisTurn = true;
}
