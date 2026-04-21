#include "models/io/ConfigReader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <stdexcept>

#include "models/tile/RailRoadTile.hpp"
#include "models/tile/UtilityTile.hpp"

Reader::Reader(std::string folderName)
    : folderName(folderName),
      propertyConfigFileName("property.txt"),
      railRoadConfigFileName("railroad.txt"),
      utilityConfigFileName("utility.txt"),
      taxConfigFileName("tax.txt"),
      specialTileConfigFileName("special.txt"),
      othersConfigFileName("misc.txt")
{
}

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

void Reader::readRailRoad()
{
    std::ifstream file(buildPath(railRoadConfigFileName).c_str());
    if (!file.is_open())
    {
        throw std::runtime_error("Gagal membuka file railroad config.");
    }

    std::string header;
    std::getline(file, header);

    std::map<int, int> railroadRentMap;
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::istringstream iss(line);
        int count = 0;
        int rent = 0;

        if (!(iss >> count >> rent))
        {
            throw std::runtime_error("Format railroad.txt tidak valid.");
        }

        railroadRentMap[count] = rent;
    }

    if (railroadRentMap.empty())
    {
        throw std::runtime_error("railroad.txt kosong atau tidak valid.");
    }

    RailRoadTile::setRailRoadRentPrices(railroadRentMap);
}

void Reader::readUtility()
{
    std::ifstream file(buildPath(utilityConfigFileName).c_str());
    if (!file.is_open())
    {
        throw std::runtime_error("Gagal membuka file utility config.");
    }

    std::string header;
    std::getline(file, header);

    std::map<int, int> utilityFactorMap;
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::istringstream iss(line);
        int count = 0;
        int factor = 0;

        if (!(iss >> count >> factor))
        {
            throw std::runtime_error("Format utility.txt tidak valid.");
        }

        utilityFactorMap[count] = factor;
    }

    if (utilityFactorMap.empty())
    {
        throw std::runtime_error("utility.txt kosong atau tidak valid.");
    }

    UtilityTile::setUtilityFactor(utilityFactorMap);
}

Board Reader::readProperty()
{
    // TODO
}

void Reader::readTax()
{
    // TODO
}

void Reader::readSpecial()
{
    // TODO
}

void Reader::readMisc()
{
    // TODO
}