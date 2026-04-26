#include "core/Monopoly.hpp"
#include "models/board/Board.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/tile/property_tile/StreetTile.hpp"
#include "models/tile/property_tile/UtilityTile.hpp"
#include "utils/Formatter.hpp"
#include "controllers/GameConfig.hpp"
#include "models/player/Player.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

class CLICommand : public CommandInterface {
    private:
        static std::string toUpper(std::string s) {
            std::transform(s.begin(), s.end(), s.begin(),
                [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
            return s;
        }

        static bool askYesNo(const std::string& prompt) {
            while (true) {
                std::cout << prompt << " (y/n): ";
                std::string input;
                std::getline(std::cin, input);
                input = toUpper(input);

                if (input == "Y" || input == "YES") {
                    return true;
                }
                if (input == "N" || input == "NO") {
                    return false;
                }

                std::cout << "Input tidak valid. Masukkan y atau n.\n";
            }
        }

        static bool parseInt(const std::string& token, int& value) {
            std::istringstream iss(token);
            iss >> value;
            return !iss.fail() && iss.eof();
        }

    public:
        Command getCommand() override;

        int getInt(int lowerBound, int upperBound) override;

        int getAuctionBidOrPass(int currentBid, int playerBalance) override;

        bool askWantToSellAllBuildings(std::string question) override;

        std::string askFolderForConfig() override;

        int askNumOfPlayer() override;

        std::string askPlayerUsername(int i) override;

        bool askWantToLoadState() override;

        std::string askStateFilename() override;

        std::string getTileToGetFestival() override;

        bool askWantToBuyProperty() override;

        bool getBool(std::string message) override;

        std::string getString() override;
};