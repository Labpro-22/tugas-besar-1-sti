#include "controllers/SkillCardCoordinator.hpp"

#include <iostream>
#include <utility>

SkillCardCoordinator::SkillCardCoordinator(Deck<SkillCard>& skillCardDeck,
                                        GameViewInterface& view,
                                        CommandInterface& command)
    : skillCardDeck_(skillCardDeck), view_(view), command_(command) {}

void SkillCardCoordinator::processPickAndDropSkillCard(Player& player) {
    std::unique_ptr<SkillCard> newObtainedCard = skillCardDeck_.drawDeck();
    view_.showMessage("KARTU YANG DIDAPATKAN ..........");
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

    player.getInventory().addSkillCards(std::move(newObtainedCard));
    view_.showMessage("Kartu X berhasil dibuang, Kartu Y kembali diterima!\n");
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

    view_.showMessage("Silahkan pilih kartu yang mau kamu pakai!\n");

    int selected = command_.getInt(1, player.getSkillCardCount());
    std::size_t selectedIndex = static_cast<std::size_t>(selected - 1);
    const SkillCard* selectedCard = player.getSkillCardAt(selectedIndex);

    if (player.isInJail()) {
        if (dynamic_cast<const MoveCard*>(selectedCard) != nullptr ||
            dynamic_cast<const TeleportCard*>(selectedCard) != nullptr ||
            dynamic_cast<const LassoCard*>(selectedCard) != nullptr) {
            view_.showMessage("Saat di penjara, kartu Move/Teleport tidak dapat digunakan.\n");
            return;
        }
    }

    std::unique_ptr<SkillCard> usedCard = player.removeSkillCardAt(selectedIndex);
    usedCard->activate(player);
    view_.showMessage("Kartu " + usedCard->getName() + " telah dipakai.\n");

    skillCardDeck_.pushToDiscard(std::move(usedCard));
    hasUsedSkillCardThisTurn = true;
}
