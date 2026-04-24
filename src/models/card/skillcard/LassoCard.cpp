#include "models/card/skillcard/LassoCard.hpp"
#include "models/player/Player.hpp"

LassoCard::LassoCard()
    : SkillCard("Lasso Card", "Menarik satu lawan di depan ke petak pemain saat ini") {}

void LassoCard::activate(Player& owner) {
    const int ownerPos = owner.getPosition();

    Player* nearestAhead = nullptr;
    int nearestAheadPos = std::numeric_limits<int>::max();

    Player* wrapCandidate = nullptr;
    int wrapCandidatePos = std::numeric_limits<int>::max();

    for (Player* candidate : Player::getAllPlayers()) {
        if (candidate == nullptr || candidate == &owner) {
            continue;
        }
        if (candidate->isBankrupt()) {
            continue;
        }

        const int pos = candidate->getPosition();
        if (pos > ownerPos) {
            if (pos < nearestAheadPos) {
                nearestAheadPos = pos;
                nearestAhead = candidate;
            }
        } else {
            if (pos < wrapCandidatePos) {
                wrapCandidatePos = pos;
                wrapCandidate = candidate;
            }
        }
    }

    Player* target = nearestAhead != nullptr ? nearestAhead : wrapCandidate;
    if (target == nullptr) {
        std::cout << "[LASSO] Tidak ada lawan yang bisa ditarik.\n";
        return;
    }

    target->setPosition(ownerPos);
    std::cout << "[LASSO] " << target->getUsername() << " ditarik ke petak " << ownerPos << ".\n";
}
