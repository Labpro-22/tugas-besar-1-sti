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

    view_.showMessage("Kartu yang didapatkan: " + newObtainedCard->getName() + "\n");
    bool throwNewObtained = false;

    try {
        player.getInventory().addSkillCards(std::move(newObtainedCard));
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        view_.showMessage("Silahkan pilih yang mau dibuang terlebih dahulu\n");

        int selected = command_.getInt(1, player.getSkillCardCount() + 1);
        if (selected == player.getSkillCardCount() + 1) {
            throwNewObtained = true;
        }

        if (throwNewObtained) {
            skillCardDeck_.pushToDiscard(std::move(newObtainedCard));
        } else {
            std::unique_ptr<SkillCard> thrownAwayCard = player.getInventory().removeSkillCardAt(selected);
            player.getInventory().addSkillCards(std::move(newObtainedCard));
            skillCardDeck_.pushToDiscard(std::move(thrownAwayCard));
        }
        return;
    }

    view_.showMessage("Kartu berhasil ditambahkan ke inventory.\n");
}

void SkillCardCoordinator::processSkillCardUse(Player& player, bool& hasUsedSkillCardThisTurn) {
    if (hasUsedSkillCardThisTurn) {
        view_.showMessage("Kartu kemampuan hanya boleh dipakai maksimal 1 kali per giliran.\n");
        return;
    }

    if (player.getSkillCardCount() == 0) {
        view_.showMessage("Kamu tidak punya kartu kemampuan untuk digunakan.\n");
        return;
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
    view_.showMessage("Silahkan pilih kartu yang mau kamu pakai!\n");

    int selected = command_.getInt(1, player.getSkillCardCount());
    std::size_t selectedIndex = static_cast<std::size_t>(selected - 1);
    const SkillCard* selectedCard = player.getSkillCardAt(selectedIndex);

    if (selectedCard == nullptr) {
        view_.showMessage("Kartu yang dipilih tidak valid.\n");
        return;
    }

    if (player.isInJail()) {
        if (dynamic_cast<const MoveCard*>(selectedCard) != nullptr ||
            dynamic_cast<const TeleportCard*>(selectedCard) != nullptr ||
            dynamic_cast<const LassoCard*>(selectedCard) != nullptr) {
            view_.showMessage("Saat di penjara, kartu Move/Teleport tidak dapat digunakan.\n");
            return;
        }
    }

    int balanceBefore = player.getBalance();
    int positionBefore = player.getPosition();

    std::unique_ptr<SkillCard> usedCard = player.removeSkillCardAt(selectedIndex);
    if (!usedCard) {
        view_.showMessage("Gagal mengambil kartu dari inventory.\n");
        return;
    }

    usedCard->activate(player);
    view_.showMessage("Kartu " + usedCard->getName() + " telah dipakai.\n");
    view_.showMessage("Efek diterapkan | Posisi: " + std::to_string(positionBefore) + " -> " + std::to_string(player.getPosition()) +
                    " | Balance: " + std::to_string(balanceBefore) + " -> " + std::to_string(player.getBalance()) + "\n");

    skillCardDeck_.pushToDiscard(std::move(usedCard));
    hasUsedSkillCardThisTurn = true;
}
