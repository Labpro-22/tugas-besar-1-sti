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

class CLIView : public GameViewInterface {
    private:
        Board* board_ = nullptr;

        static std::string propertyKind(const PropertyTile& property) {
            if (dynamic_cast<const StreetTile*>(&property) != nullptr) {
                return "Street";
            }
            if (dynamic_cast<const RailRoadTile*>(&property) != nullptr) {
                return "RailRoad";
            }
            if (dynamic_cast<const UtilityTile*>(&property) != nullptr) {
                return "Utility";
            }
            return "Property";
        }

        int boardSize() const;

    public:
        void showMessage(std::string message) override;

        void setBoardContext(Board* board) override;
        void cetakPapan(Player& pl, int currentTurn) override;
        void cetakProperti(Player* player) override;
        void cetakAkta(const std::string& kodePetak, bool showStatus = true) override;
};