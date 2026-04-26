#include "models/card/skillcard/LassoCard.hpp"
#include "models/player/Player.hpp"

LassoCard::LassoCard()
    : SkillCard("Lasso Card", "Menarik satu lawan di depan ke petak pemain saat ini") {}

void LassoCard::activate(Player& owner) {
    // const int ownerPos = owner.getPosition();

    // int targetTilePos = std::numeric_limits<int>::max();
    // bool foundAhead = false;

    // for (Player* candidate : owner.getAllPlayers()) {
    //     if (candidate == nullptr || candidate == &owner) {
    //         continue;
    //     }
    //     if (candidate->isBankrupt()) {
    //         continue;
    //     }

    //     const int pos = candidate->getPosition();
    //     if (pos > ownerPos) {
    //         if (!foundAhead || pos < targetTilePos) {
    //             foundAhead = true;
    //             targetTilePos = pos;
    //         }
    //         continue;
    //     }

    //     if (!foundAhead && pos < targetTilePos) {
    //         targetTilePos = pos;
    //     }
    // }

    // if (targetTilePos == std::numeric_limits<int>::max()) {
    //     std::cout << "[LASSO] Tidak ada lawan yang bisa ditarik.\n";
    //     return;
    // }

    // int pulledCount = 0;
    // for (Player* candidate : owner.getAllPlayers()) {
    //     if (candidate == nullptr || candidate == &owner) {
    //         continue;
    //     }
    //     if (candidate->isBankrupt()) {
    //         continue;
    //     }

    //     if (candidate->getPosition() == targetTilePos) {
    //         candidate->setPosition(ownerPos);
    //         ++pulledCount;
    //         std::cout << "[LASSO] " << candidate->getUsername() << " ditarik ke petak " << ownerPos << ".\n";
    //     }
    // }

    // if (pulledCount == 0) {
    //     std::cout << "[LASSO] Tidak ada lawan yang bisa ditarik.\n";
    // } else if (pulledCount > 1) {
    //     std::cout << "[LASSO] Total " << pulledCount << " pemain ditarik dari petak " << targetTilePos << ".\n";
    // }
}
