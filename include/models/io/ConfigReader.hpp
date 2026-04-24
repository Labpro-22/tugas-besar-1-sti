#pragma once
#include <iostream>
#include "../board/Board.hpp"

#include "../models/tile/property_tile/RailRoadTile.hpp"
#include "../models/tile/property_tile/UtilityTile.hpp"
#include "../models/tile/property_tile/StreetTile.hpp"
#include "../models/tile/property_tile/PLNTile.hpp"
#include "../models/tile/property_tile/PAMTile.hpp"
#include "../models/tile/action_tile/ActionTile.hpp"
#include "../models/tile/action_tile/FestivalTile.hpp"
#include "../models/tile/action_tile/card_tile/CardTile.hpp"
#include "../models/tile/action_tile/card_tile/ChanceTile.hpp"
#include "../models/tile/action_tile/card_tile/CommunityChestTile.hpp"
#include "../models/tile/action_tile/special_tile/FreeParkingTile.hpp"
#include "../models/tile/action_tile/special_tile/GoTile.hpp"
#include "../models/tile/action_tile/special_tile/GoToJailTile.hpp"
#include "../models/tile/action_tile/special_tile/PrisonTile.hpp"
#include "../models/tile/action_tile/special_tile/SpecialTile.hpp"
#include "../models/tile/action_tile/tax_tile/IncomeTaxTile.hpp"
#include "../models/tile/action_tile/tax_tile/LuxuryGoodsTaxTile.hpp"
#include "../models/tile/action_tile/tax_tile/TaxTile.hpp"

#include "models/exception/ConfigException.hpp"
#include "models/exception/LoadConfigFailed.hpp"

// develop guidance :

class Reader
{
    private:
        std::string folderName;
        std::string propertyConfigFileName;
        std::string railRoadConfigFileName;
        std::string utilityConfigFileName;
        std::string taxConfigFileName;
        std::string actionConfigFileName;
        std::string specialTileConfigFileName;
        std::string othersConfigFileName;

        // untuk readProperty
        static int goSalary_;
        static int jailFine_;
        static int maxTurn_;
        static int startingBalance_;

        // build folderName + fileName
        std::string buildPath(const std::string& fileName) const;

    public:
        /*Buka folder sekaligus isi itu filename masing"*/
        Reader(std::string folderName);

        // load config jadi
        ~Reader();

        // baca property
        std::vector<Tile*> readProperty();

        // set map static yang jadi attribut kelas railroad
        void readRailRoad();

        // set map static yang jadi attribut kelas util
        void readUtility();
        
        // pake set static aja
        void readTax();

        std::vector<Tile*> readAction();

        Board createBoard();

        void readSpecial();

        void readMisc();

        static int getGoSalary();
        static int getJailFine();
        static int getMaxTurn();
        static int getStartingBalance();
};

