#pragma once
#include "models/player/Player.hpp"

#include <string>

class ChanceCard {
    public:
        enum Instruction {
            GoToNearestStation,
            MoveBackThreeTiles,
            GoToJail
        };

        static const int kInstructionCount = 3;

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
