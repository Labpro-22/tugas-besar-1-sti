#include "models/card/ChanceCard.hpp"

#include <random>

ChanceCard::ChanceCard()
    : ChanceCard(GoToNearestStation) {}

ChanceCard::ChanceCard(Instruction instruction)
    : instruction_(instruction),
      description_(instructionToDescription(instruction)) {}

void ChanceCard::useChance(Player& owner) const {
    // TODO: Integrasi Player/Board/Monopoly.
    (void)owner;
}

ChanceCard::Instruction ChanceCard::getInstruction() const {
    return instruction_;
}

const std::string& ChanceCard::getDescription() const {
    return description_;
}

ChanceCard ChanceCard::randomCard() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, kInstructionCount - 1);

    return ChanceCard(static_cast<Instruction>(dist(gen)));
}

std::string ChanceCard::instructionToDescription(Instruction instruction) {
    switch (instruction) {
        case GoToNearestStation:
            return "Pergi ke stasiun terdekat.";
        case MoveBackThreeTiles:
            return "Mundur 3 petak.";
        case GoToJail:
            return "Masuk Penjara.";
        default:
            return "Kartu Kesempatan tidak dikenal.";
    }
}
