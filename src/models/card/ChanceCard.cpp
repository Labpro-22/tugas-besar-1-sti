#include "models/card/ChanceCard.hpp"

#include <random>

ChanceCard::ChanceCard()
    : ChanceCard(GoToNearestStation) {}

ChanceCard::ChanceCard(Instruction instruction)
    : instruction_(instruction),
      description_(instructionToDescription(instruction)) {}

void ChanceCard::useChance(Player& owner) const {
    switch (instruction_) {
        case GoToNearestStation:
            // TODO: Integrasi board untuk cari stasiun terdekat lalu pindahkan posisi owner.
            break;

        case MoveBackThreeTiles:
            // TODO: Integrasi board untuk pergerakan mundur dengan wrapping posisi.
            owner.setPosition(owner.getPosition() - 3);
            break;

        case GoToJail:
            owner.setStatus(Player::JAILED);
            owner.resetJailTurn();
            // TODO: Integrasi board untuk set posisi owner ke tile penjara.
            break;

        default:
            break;
    }
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
