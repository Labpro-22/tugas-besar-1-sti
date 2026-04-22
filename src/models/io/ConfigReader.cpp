#include "models/io/ConfigReader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <stdexcept>
#include <algorithm>

Reader::Reader(std::string folderName)
    : folderName(folderName), 
    propertyConfigFileName("property.txt"), 
    railRoadConfigFileName("railroad.txt"), 
    utilityConfigFileName("utility.txt"), 
    taxConfigFileName("tax.txt"), 
    actionConfigFileName("aksi.txt"),
    specialTileConfigFileName("special.txt"), 
    othersConfigFileName("misc.txt"), 
    goSalary_(0), jailFine_(0), maxTurn_(0), startingBalance_(0) {}

Reader::~Reader() {}

std::string Reader::buildPath(const std::string& fileName) const
{
    if (folderName.empty())
    {
        return fileName;
    }

    if (folderName.back() == '/' || folderName.back() == '\\')
    {
        return folderName + fileName;
    }

    return folderName + "/" + fileName;
}

int Reader::getGoSalary() const
{
    return goSalary_;
}

int Reader::getJailFine() const
{
    return jailFine_;
}

int Reader::getMaxTurn() const
{
    return maxTurn_;
}

int Reader::getStartingBalance() const
{
    return startingBalance_;
}

void Reader::readRailRoad()
{
    std::ifstream file(buildPath(railRoadConfigFileName).c_str());
    if (!file.is_open())
    {
        throw LoadConfigFailed("Gagal membuka file railroad config: " + buildPath(railRoadConfigFileName));
    }

    std::string header;
    if (!std::getline(file, header))
    {
        throw ConfigException(2, "railroad.txt kosong.");
    }

    std::map<int, int> railroadRentMap;
    std::string line;
    bool hasData = false;

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        hasData = true;

        std::istringstream iss(line);
        int count = 0;
        int rent = 0;

        if (!(iss >> count >> rent))
        {
            throw ConfigException(3, "Format railroad.txt tidak valid.");
        }

        railroadRentMap[count] = rent;
    }

    if (!hasData)
    {
        throw ConfigException(2, "railroad.txt tidak memiliki data.");
    }

    if (railroadRentMap.empty())
    {
        throw ConfigException(2, "railroad.txt kosong atau tidak valid.");
    }

    RailRoadTile::setRailRoadRentPrices(railroadRentMap);
}

void Reader::readUtility()
{
    std::ifstream file(buildPath(utilityConfigFileName).c_str());
    if (!file.is_open())
    {
        throw LoadConfigFailed("Gagal membuka file utility config: " + buildPath(utilityConfigFileName));
    }

    std::string header;
    if (!std::getline(file, header))
    {
        throw ConfigException(2, "utility.txt kosong.");
    }

    std::map<int, int> utilityFactorMap;
    std::string line;
    bool hasData = false;

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        hasData = true;

        std::istringstream iss(line);
        int count = 0;
        int factor = 0;

        if (!(iss >> count >> factor))
        {
            throw ConfigException(3, "Format utility.txt tidak valid.");
        }

        utilityFactorMap[count] = factor;
    }

    if (!hasData)
    {
        throw ConfigException(2, "utility.txt tidak memiliki data.");
    }

    if (utilityFactorMap.empty())
    {
        throw ConfigException(2, "utility.txt kosong atau tidak valid.");
    }

    UtilityTile::setUtilityFactor(utilityFactorMap);
}

std::vector<Tile*> Reader::readProperty()
{
    std::vector<Tile*> propertyTiles;
    std::ifstream file(buildPath(propertyConfigFileName).c_str());
    if (!file.is_open())
    {
        throw LoadConfigFailed("Gagal membuka file property config: " + buildPath(propertyConfigFileName));
    }

    std::string header;
    if (!std::getline(file, header))
    {
        throw ConfigException(2, "property.txt kosong.");
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty()) continue;
        std::istringstream iss(line);

        int tileId, purchasePrice, mortgageValue;
        std::string letterCode, tileName, tileType, colourBlock;

        if (!(iss >> tileId >> letterCode >> tileName >> tileType >> colourBlock >> purchasePrice >> mortgageValue)) continue;

        Tile* newTile = nullptr;
        if (tileType == "STREET")
        {
            int houseUpgradePrice, hotelUpgradePrice;
            if (!(iss >> houseUpgradePrice >> hotelUpgradePrice)) {
                throw ConfigException(3, "Format upgrade STREET tidak valid pada ID: " + std::to_string(tileId));
            }
            std::map<int, int> buildPriceMap;
            buildPriceMap[1] = houseUpgradePrice;
            buildPriceMap[2] = houseUpgradePrice;
            buildPriceMap[3] = houseUpgradePrice;
            buildPriceMap[4] = houseUpgradePrice;
            buildPriceMap[5] = hotelUpgradePrice;

            std::map<int, int> rentPriceMap;
            for (int level = 0; level <= 5; ++level) {
                int rentPrice;
                if (!(iss >> rentPrice)) {
                    throw ConfigException(3, "Format RENT STREET tidak valid pada ID: " + std::to_string(tileId) + " untuk Level " + std::to_string(level));
                }
                rentPriceMap[level] = rentPrice;
            }
            newTile = new StreetTile(tileId, letterCode, tileName, colourBlock, purchasePrice, mortgageValue, rentPriceMap, buildPriceMap);
        }
        else if (tileType == "RAILROAD") {
            newTile = new RailRoadTile(tileId, letterCode, tileName, colourBlock, purchasePrice, mortgageValue);
        }
        else if (tileType == "UTILITY") 
        {
            if (letterCode == "PLN") 
            {
                newTile = new PLNTile(tileId, letterCode, tileName, colourBlock, purchasePrice, mortgageValue);
            } 
            else if (letterCode == "PAM") 
            {
                newTile = new PAMTile(tileId, letterCode, tileName, colourBlock, purchasePrice, mortgageValue);
            } 
            else 
            {
                throw ConfigException(3, "Kode UTILITY tidak dikenali pada ID: " + std::to_string(tileId));
            }
        }

        if (newTile != nullptr) {
            propertyTiles.push_back(newTile);
        }
    }
    return propertyTiles;
}

void Reader::readTax()
{
    std::ifstream file(buildPath(taxConfigFileName).c_str());
    if (!file.is_open())
    {
        throw LoadConfigFailed("Gagal membuka file tax config: " + buildPath(taxConfigFileName));
    }

    std::string header;
    if (!std::getline(file, header))
    {
        throw ConfigException(2, "tax.txt kosong.");
    }

    std::string line;
    if (!std::getline(file, line))
    {
        throw ConfigException(2, "Isi tax.txt tidak ditemukan.");
    }

    std::istringstream iss(line);

    int pphFlat = 0;
    float pphPercentage = 0.0f;
    int pbmFlat = 0;

    if (!(iss >> pphFlat >> pphPercentage >> pbmFlat))
    {
        throw ConfigException(3, "Format tax.txt tidak valid.");
    }

    if (pphFlat < 0 || pphPercentage < 0.0f || pbmFlat < 0)
    {
        throw ConfigException(4, "Nilai tax.txt tidak boleh negatif.");
    }

    IncomeTaxTile::setPPHFlatCost(pphFlat);
    IncomeTaxTile::setTaxPercentage(pphPercentage);
    LuxuryGoodsTaxTile::setPBMFlatCost(pbmFlat);
}

std::vector<Tile*> Reader::readAction() 
{
    std::vector<Tile*> actionTiles;
    std::ifstream file(buildPath(actionConfigFileName).c_str());
    
    if (!file.is_open())
    {
        throw LoadConfigFailed("Gagal membuka file tax config: " + buildPath(actionConfigFileName));
    }

    std::string header;
    if (!std::getline(file, header))
    {
        throw ConfigException(2, "aksi.txt kosong.");
    }

    std::string line;
    if (!std::getline(file, line))
    {
        throw ConfigException(2, "Isi aksi.txt tidak ditemukan.");
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        std::istringstream iss(line);
        int tileId;
        std::string letterCode, tileName, tileType, colourBlock;

        if (!(iss >> tileId >> letterCode >> tileName >> tileType >> colourBlock)) {
            throw ConfigException(3, "Format aksi.txt tidak valid pada ID: " + std::to_string(tileId));
        }

        Tile* newTile = nullptr;

        // TODO: GoSalary masuk ke mana?
        if (letterCode == "GO")       newTile = new GoTile(tileId, letterCode, tileName, goSalary_);
        else if (letterCode == "DNU") newTile = new CommunityChestTile(tileId, letterCode, tileName);
        else if (letterCode == "KSP") newTile = new ChanceTile(tileId, letterCode, tileName);
        else if (letterCode == "PPH") newTile = new IncomeTaxTile(tileId, letterCode, tileName);
        else if (letterCode == "PBM") newTile = new LuxuryGoodsTaxTile(tileId, letterCode, tileName);
        else if (letterCode == "FES") newTile = new FestivalTile(tileId, letterCode, tileName);
        // TODO: jailFine masuk ke mana???
        else if (letterCode == "PEN") newTile = new PrisonTile(tileId, letterCode, tileName, jailFine_);
        else if (letterCode == "PPJ") newTile = new GoToJailTile(tileId, letterCode, tileName);
        else if (letterCode == "BBP") newTile = new FreeParkingTile(tileId, letterCode, tileName);
        else {
            throw ConfigException(3, "Kode Action Tile tidak valid pada ID: " + std::to_string(tileId));
        }

        if (newTile != nullptr) {
            actionTiles.push_back(newTile);
        }
    }

    return actionTiles;
}

Board Reader::createBoard()
{
    std::vector<Tile*> propertyTiles = readProperty();
    std::vector<Tile*> actionTiles = readAction();
    std::vector<Tile*> allTiles;
    allTiles.insert(allTiles.end(), propertyTiles.begin(), propertyTiles.end());
    allTiles.insert(allTiles.end(), actionTiles.begin(), actionTiles.end());

    std::sort(allTiles.begin(), allTiles.end(), [](Tile* a, Tile* b) {
        return a->getTileID() < b->getTileID();
    });

    int totalSize = allTiles.size();
    Board board(totalSize);

    for (int i = 0; i < totalSize; ++i) {
        int expectedId = i + 1;
        int actualId = allTiles[i]->getTileID();

        if (actualId != expectedId) {
            throw ConfigException(3, "Terdapat ID duplikat atau hilang di sekitar ID " + std::to_string(actualId));
        }

        board.addTile(*(allTiles[i]));

        std::string letterCode = allTiles[i]->getLetterCode();
        if (letterCode == "GO") {
            board.setStartPosition(expectedId);
        } else if (letterCode == "PEN") {
            board.setJailPosition(expectedId);
        }
    }

    return board;
}

void Reader::readSpecial()
{
    std::ifstream file(buildPath(specialTileConfigFileName).c_str());
    if (!file.is_open())
    {
        throw LoadConfigFailed("Gagal membuka file special config: " + buildPath(specialTileConfigFileName));
    }

    std::string header;
    if (!std::getline(file, header))
    {
        throw ConfigException(2, "special.txt kosong");
    }

    std::string line;
    if (!std::getline(file, line))
    {
        throw ConfigException(2, "special.txt tidak memiliki baris data");
    }

    std::istringstream iss(line);
    int salary = 0;
    int fine = 0;

    if (!(iss >> salary >> fine))
    {
        throw ConfigException(3, "Format tidak valid di special.txt");
    }

    if (salary < 0 || fine < 0)
    {
        throw ConfigException(4, "Nilai negatif tidak diizinkan di special.txt");
    }

    goSalary_ = salary;
    jailFine_ = fine;
}

void Reader::readMisc()
{
    std::ifstream file(buildPath(othersConfigFileName).c_str());
    if (!file.is_open())
    {
        throw LoadConfigFailed(buildPath(othersConfigFileName));
    }

    std::string header;
    if (!std::getline(file, header))
    {
        throw ConfigException(2, "misc.txt kosong");
    }

    std::string line;
    if (!std::getline(file, line))
    {
        throw ConfigException(2, "misc.txt tidak memiliki baris data");
    }

    std::istringstream iss(line);
    int maxTurn = 0;
    int saldoAwal = 0;

    if (!(iss >> maxTurn >> saldoAwal))
    {
        throw ConfigException(3, "Format tidak valid di misc.txt");
    }

    if (saldoAwal < 0)
    {
        throw ConfigException(4, "Saldo awal tidak boleh negatif di misc.txt");
    }

    // maxTurn boleh negatif -> permainan berakhir dengan salah satu pemain tidak bankrut 
    maxTurn_ = maxTurn;
    startingBalance_ = saldoAwal;
}