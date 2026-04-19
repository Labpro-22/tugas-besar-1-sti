#include "models/card/CommunityChestCard.hpp"

#include <random>

CommunityChestCard::CommunityChestCard()
    : CommunityChestCard(BirthdayCollect100FromEachPlayer) {}

CommunityChestCard::CommunityChestCard(Instruction instruction)
    : instruction_(instruction),
      description_(instructionToDescription(instruction)) {}

void CommunityChestCard::useCommunityChest(Player& owner, const std::vector<Player*>& allPlayers) const
{
    // TODO: Integrasi Player + Monopoly
    (void)owner;
    (void)allPlayers;
}

CommunityChestCard::Instruction CommunityChestCard::getInstruction() const
{
    return instruction_;
}

const std::string& CommunityChestCard::getDescription() const
{
    return description_;
}

CommunityChestCard CommunityChestCard::randomCard()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, kInstructionCount - 1);

    return CommunityChestCard(static_cast<Instruction>(dist(gen)));
}

std::string CommunityChestCard::instructionToDescription(Instruction instruction)
{
    switch (instruction)
    {
        case BirthdayCollect100FromEachPlayer:
            return "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain.";
        case DoctorFeePay700:
            return "Biaya dokter. Bayar M700.";
        case ElectionPay200ToEachPlayer:
            return "Anda mau nyaleg. Bayar M200 kepada setiap pemain.";
        default:
            return "Kartu Dana Umum tidak dikenal.";
    }
}
