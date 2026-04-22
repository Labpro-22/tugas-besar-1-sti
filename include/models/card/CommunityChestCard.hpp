#pragma once

#include <string>
#include <memory>
#include <vector>

#include "models/player/Player.hpp"

class CommunityChestCard {
    public:
        enum Instruction {
            BirthdayCollect100FromEachPlayer,
            DoctorFeePay700,
            ElectionPay200ToEachPlayer
        };

        static const int kInstructionCount = 3;

        CommunityChestCard();
        CommunityChestCard(Instruction instruction);

        void useCommunityChest(Player& owner, const std::vector<std::unique_ptr<Player>>& allPlayers) const;

        Instruction getInstruction() const;
        const std::string& getDescription() const;

        static CommunityChestCard randomCard();

    private:
        Instruction instruction_;
        std::string description_;

        static std::string instructionToDescription(Instruction instruction);
};
