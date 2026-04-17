#pragma once

#include <string>
#include <vector>

class CommunityChestCard {
public:
    enum Instruction {
        BirthdayCollect100FromEachPlayer,
        DoctorFeePay700,
        ElectionPay200ToEachPlayer
    };

    static constexpr int kInstructionCount = 3;

    CommunityChestCard();
    CommunityChestCard(Instruction instruction);

    void useCommunityChest(class Player& owner, const std::vector<class Player*>& allPlayers) const;

    Instruction getInstruction() const;
    const std::string& getDescription() const;

    static CommunityChestCard randomCard();

private:
    Instruction instruction_;
    std::string description_;

    static std::string instructionToDescription(Instruction instruction);
};
