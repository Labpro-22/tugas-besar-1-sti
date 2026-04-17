#pragma once

#include <string>

class ChanceCard {
public:
    enum Instruction {
        GoToNearestStation,
        MoveBackThreeTiles,
        GoToJail
    };

    static constexpr int kInstructionCount = 3;

    ChanceCard();
    ChanceCard(Instruction instruction);

    void useChance(class Player& owner) const;

    Instruction getInstruction() const;
    const std::string& getDescription() const;

    static ChanceCard randomCard();

private:
    Instruction instruction_;
    std::string description_;

    static std::string instructionToDescription(Instruction instruction);
};
