#include "models/io/ConfigReader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <stdexcept>

#include "models/tile/RailRoadTile.hpp"
#include "models/tile/UtilityTile.hpp"
#include "models/tile/StreetTile.hpp"
#include "models/tile/PLNTile.hpp"
#include "models/tile/PAMTile.hpp"
#include "models/tile/ActionTile.hpp"
#include "models/tile/SpecialTile.hpp"

#include "models/exception/ConfigException.hpp"
#include "models/exception/LoadConfigFailed.hpp"

Reader::Reader(std::string folderName)
    : folderName(folderName), 
    propertyConfigFileName("property.txt"), 
    railRoadConfigFileName("railroad.txt"), 
    utilityConfigFileName("utility.txt"), 
    taxConfigFileName("tax.txt"), 
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
        throw ConfigException(1002, "railroad.txt kosong.");
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
            throw ConfigException(1003, "Format railroad.txt tidak valid.");
        }

        railroadRentMap[count] = rent;
    }

    if (!hasData)
    {
        throw ConfigException(1002, "railroad.txt tidak memiliki data.");
    }

    if (railroadRentMap.empty())
    {
        throw ConfigException(1002, "railroad.txt kosong atau tidak valid.");
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
        throw ConfigException(1002, "utility.txt kosong.");
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
            throw ConfigException(1003, "Format utility.txt tidak valid.");
        }

        utilityFactorMap[count] = factor;
    }

    if (!hasData)
    {
        throw ConfigException(1002, "utility.txt tidak memiliki data.");
    }

    if (utilityFactorMap.empty())
    {
        throw ConfigException(1002, "utility.txt kosong atau tidak valid.");
    }

    UtilityTile::setUtilityFactor(utilityFactorMap);
}

Board Reader::readProperty()
{
    // TODO
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
        throw ConfigException(1002, "tax.txt kosong.");
    }

    std::string line;
    if (!std::getline(file, line))
    {
        throw ConfigException(1002, "Isi tax.txt tidak ditemukan.");
    }

    std::istringstream iss(line);

    int pphFlat = 0;
    float pphPercentage = 0.0f;
    int pbmFlat = 0;

    if (!(iss >> pphFlat >> pphPercentage >> pbmFlat))
    {
        throw ConfigException(1003, "Format tax.txt tidak valid.");
    }

    if (pphFlat < 0 || pphPercentage < 0.0f || pbmFlat < 0)
    {
        throw ConfigException(1004, "Nilai tax.txt tidak boleh negatif.");
    }

    IncomeTaxTile::setPPHFlatCost(pphFlat);
    IncomeTaxTile::setTaxPercentage(pphPercentage);
    LuxuryGoodsTaxTile::setPBMFlatCost(pbmFlat);
}

void Reader::readSpecial()
{
    // TODO
}

void Reader::readMisc()
{
    // TODO
}