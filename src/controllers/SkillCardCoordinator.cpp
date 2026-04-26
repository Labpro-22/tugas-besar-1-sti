#include "controllers/SkillCardCoordinator.hpp"

#include <iostream>
#include <utility>

SkillCardCoordinator::SkillCardCoordinator(Deck<SkillCard>& skillCardDeck,
                                        GameViewInterface& view,
                                        CommandInterface& command)
    : skillCardDeck_(skillCardDeck), view_(view), command_(command) {}

void SkillCardCoordinator::processPickAndDropSkillCard(Player& player) {
    std::unique_ptr<SkillCard> newObtainedCard = skillCardDeck_.drawDeck();
    if (!newObtainedCard) {
        view_.showMessage("Deck kartu kemampuan kosong.\n");
        return;
    }

    view_.showMessage("\nKartu yang didapatkan: " + newObtainedCard->getName() + "\n");
    if (player.getSkillCardCount() >= 3) {
        view_.showMessage("PERINGATAN: Kamu sudah memiliki 3 kartu di tangan (Maksimal 3)!\n");
        view_.showMessage("Kamu diwajibkan membuang 1 kartu.\n");

        for (std::size_t i = 0; i < player.getSkillCardCount(); ++i) {
            const SkillCard* card = player.getSkillCardAt(i);
            if (card == nullptr) {
                view_.showMessage(std::to_string(i + 1) + ". [Kartu tidak valid]\n");
                continue;
            }
            view_.showMessage(std::to_string(i + 1) + ". " + card->getName() + " - " + card->getDescription() + "\n");
        }
        view_.showMessage("0. Batal (buang kartu baru)\n");

        int selected = command_.getInt(0, static_cast<int>(player.getSkillCardCount()));
        if (selected == 0) {
            skillCardDeck_.pushToDiscard(std::move(newObtainedCard));
            view_.showMessage("Kartu baru dibatalkan dan dikembalikan ke discard pile.\n");
            return;
        }

        std::size_t selectedIndex = static_cast<std::size_t>(selected - 1);
        std::unique_ptr<SkillCard> thrownAwayCard = player.getInventory().removeSkillCardAt(selectedIndex);
        player.getInventory().addSkillCards(std::move(newObtainedCard));
        skillCardDeck_.pushToDiscard(std::move(thrownAwayCard));
        view_.showMessage("Kartu pilihanmu dibuang. Kartu baru masuk ke inventory.\n");
        return;
    }

    player.getInventory().addSkillCards(std::move(newObtainedCard));
    view_.showMessage("Kartu berhasil ditambahkan ke inventory.\n");
}

std::unique_ptr<SkillCard> SkillCardCoordinator::processSkillCardUse(Player& player, bool& hasUsedSkillCardThisTurn) {
    if (hasUsedSkillCardThisTurn) {
        view_.showMessage("Kartu kemampuan hanya boleh dipakai maksimal 1 kali per giliran.\n");
        return nullptr;
    }

    if (player.getSkillCardCount() == 0) {
        view_.showMessage("Kamu tidak punya kartu kemampuan untuk digunakan.\n");
        return nullptr;
    }

    view_.showMessage("Daftar kartu kemampuan milikmu:\n");
    for (std::size_t i = 0; i < player.getSkillCardCount(); ++i) {
        const SkillCard* card = player.getSkillCardAt(i);
        if (card == nullptr) {
            view_.showMessage(std::to_string(i + 1) + ". [Kartu tidak valid]\n");
            continue;
        }

        view_.showMessage(std::to_string(i + 1) + ". " + card->getName() + " - " + card->getDescription() + "\n");
    }
    view_.showMessage("0. Batal\n");
    view_.showMessage("Silahkan pilih kartu yang mau kamu pakai!\n");

    int selected = command_.getInt(0, static_cast<int>(player.getSkillCardCount()));
    if (selected == 0) {
        view_.showMessage("Penggunaan kartu dibatalkan.\n");
        return nullptr;
    }

    std::size_t selectedIndex = static_cast<std::size_t>(selected - 1);
    const SkillCard* selectedCard = player.getSkillCardAt(selectedIndex);

    // Cek validasi penjara
    if (player.isInJail()) {
        if (dynamic_cast<const MoveCard*>(selectedCard) ||
            dynamic_cast<const TeleportCard*>(selectedCard) ||
            dynamic_cast<const LassoCard*>(selectedCard)) {
            view_.showMessage("Saat di penjara, kartu Move/Teleport/Lasso tidak dapat digunakan.\n");
            return nullptr;
        }
    }

    // Ambil kartu dari inventory dan kembalikan ke GameController
    std::unique_ptr<SkillCard> usedCard = player.removeSkillCardAt(selectedIndex);
    
    if (usedCard) {
        hasUsedSkillCardThisTurn = true;
        return usedCard; 
    }

    return nullptr;
}
