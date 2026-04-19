#include "models/tile/RailRoadTile.hpp"

static std::map<std::string, int> g_ownerRailroadCount;

RailRoadTile::RailRoadTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int purchasePrice, int mortgageValue)
    : PropertyTile(tileID, letterCode, tileName, colourBlock, purchasePrice, mortgageValue)
{
    if (railRoadRentPrices_.empty())
    {
        // Default tunggu reader
        railRoadRentPrices_[1] = 25;
        railRoadRentPrices_[2] = 50;
        railRoadRentPrices_[3] = 100;
        railRoadRentPrices_[4] = 200;
    }
}

RailRoadTile::~RailRoadTile() {}

void RailRoadTile::onLand(Player* player)
{
    if (player != nullptr)
    {
        executeTile(*player);
    }
}

void RailRoadTile::executeTile(Player& p)
{
    std::ostringstream tagBuilder;
    tagBuilder << &p;
    const std::string currentPlayerTag = tagBuilder.str();

    std::cout << "[Landed] Anda tiba di stasiun: " << tileName_ << std::endl;

    if (propertyStatus_ == BANK || ownerUsername_ == "BANK")
    {
        ownerUsername_ = currentPlayerTag;
        propertyStatus_ = OWNED;
        ++g_ownerRailroadCount[ownerUsername_];

        std::cout << "[RAILROAD] Otomatis dimiliki pemain pertama yang tiba." << std::endl;
        std::cout << "[RAILROAD] Jumlah stasiun pemain ini: " << g_ownerRailroadCount[ownerUsername_] << std::endl;
        return;
    }

    if (propertyStatus_ == MORTGAGED)
    {
        std::cout << "[RAILROAD] Stasiun sedang digadaikan, tidak ada sewa." << std::endl;
        return;
    }

    if (ownerUsername_ == currentPlayerTag)
    {
        std::cout << "[RAILROAD] Ini milik Anda." << std::endl;
        return;
    }

    const int ownerRailroadCount = std::max(1, g_ownerRailroadCount[ownerUsername_]);

    int rent = 25;
    if (!railRoadRentPrices_.empty())
    {
        auto it = railRoadRentPrices_.find(ownerRailroadCount);
        if (it != railRoadRentPrices_.end())
        {
            rent = it->second;
        }
        else
        {
            rent = railRoadRentPrices_.rbegin()->second;
        }
    }

    std::cout << "[RAILROAD] Pemilik memiliki " << ownerRailroadCount << " stasiun." << std::endl;
    std::cout << "[RAILROAD] Sewa yang harus dibayar: " << rent << std::endl;
    std::cout << "[TODO] Integrasi saldo" << std::endl;
}

// getter n setter
void RailRoadTile::setRailRoadRentPrices(const std::map<int, int>& prices)
{
    railRoadRentPrices_ = prices;
}

const std::map<int, int>& RailRoadTile::getRailRoadRentPrices()
{
    return railRoadRentPrices_;
}
