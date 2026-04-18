#include "models/tile/UtilityTile.hpp"

static std::map<std::string, int> g_ownerUtilityCount;

UtilityTile::UtilityTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : PropertyTile(tileID, letterCode, tileName, colourBlock, purchasePrice, mortgageValue)
{
    if (utilityFactor_.empty())
    {
        // Default tunggu reader
        utilityFactor_[1] = 4;
        utilityFactor_[2] = 10;
    }
}

UtilityTile::~UtilityTile() {}

void UtilityTile::executeTile(Player& p)
{
    std::cout << "[Landed] Anda tiba di utilitas: " << tileName_ << std::endl;

    std::ostringstream tagBuilder;
    tagBuilder << &p;
    const std::string currentPlayerTag = tagBuilder.str();

    if (propertyStatus_ == BANK || ownerUsername_ == "BANK")
    {
        ownerUsername_ = currentPlayerTag;
        propertyStatus_ = OWNED;
        ++g_ownerUtilityCount[ownerUsername_];

        std::cout << "[UTILITY] Otomatis dimiliki pemain pertama yang tiba." << std::endl;
        std::cout << "[UTILITY] Jumlah utility pemain ini: " << g_ownerUtilityCount[ownerUsername_] << std::endl;
        return;
    }

    if (propertyStatus_ == MORTGAGED)
    {
        std::cout << "[UTILITY] Utility sedang digadaikan, tidak ada sewa." << std::endl;
        return;
    }

    if (ownerUsername_ == currentPlayerTag)
    {
        std::cout << "[UTILITY] Ini milik Anda." << std::endl;
        return;
    }

    const int ownerUtilityCount = std::max(1, g_ownerUtilityCount[ownerUsername_]);

    int factor = 4;
    if (!utilityFactor_.empty())
    {
        auto it = utilityFactor_.find(ownerUtilityCount);
        if (it != utilityFactor_.end())
        {
            factor = it->second;
        }
        else
        {
            factor = utilityFactor_.rbegin()->second;
        }
    }

    // Dice total belum tersedia dari API Player saat ini.
    const int diceTotal = 0;
    const int rent = diceTotal * factor;

    std::cout << "[UTILITY] Pemilik memiliki " << ownerUtilityCount << " utility." << std::endl;
    std::cout << "[UTILITY] Faktor pengali: " << factor << std::endl;
    std::cout << "[UTILITY] Rumus sewa: total_dadu(" << diceTotal << ") x " << factor << " = " << rent << std::endl;
    std::cout << "[TODO] Integrasi dadu, saldo" << std::endl;
}
