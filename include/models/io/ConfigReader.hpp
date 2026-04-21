#pragma once
#include <iostream>
#include "../board/Board.hpp"

// develop guidance :

class Reader
{
    private:
        std::string folderName;
        std::string propertyConfigFileName;
        std::string railRoadConfigFileName;
        std::string utilityConfigFileName;
        std::string taxConfigFileName;
        std::string specialTileConfigFileName;
        std::string othersConfigFileName;

        // build folderName + fileName
        std::string buildPath(const std::string& fileName) const;
        
        // baca property
        Board readProperty(); // board

        // set map static yang jadi attribut kelas railroad
        void readRailRoad();

        // set map static yang jadi attribut kelas util
        void readUtility();
        
        // pake set static aja

        void readTax();

        void readSpecial();

        void readMisc();

    public:
        /*Buka folder sekaligus isi itu filename masing"*/
        Reader(std::string folderName);

        // load config jadi
        ~Reader();
};

