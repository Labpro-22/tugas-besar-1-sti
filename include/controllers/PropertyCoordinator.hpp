#pragma once

#include "models/board/Board.hpp"
#include "models/card/ChanceCard.hpp"
#include "models/dice/Dice.hpp"
#include "models/player/Player.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/tile/property_tile/StreetTile.hpp"
#include "models/tile/property_tile/UtilityTile.hpp"
#include "views/CommandInterface.hpp"
#include "views/GameViewInterface.hpp"

#include <memory>
#include <vector>

class PropertyCoordinator {
    public:
        PropertyCoordinator(std::vector<std::unique_ptr<Player>>& players,
                            Board& board,
                            Dice& dice,
                            GameViewInterface& view,
                            CommandInterface& command);

        void processFestival(Player& player);
        void processTakeChanceCard(Player& player);
        void processRedeem(Player& player);
        void processPayRent(Player& player, Tile& currentTile);
        void processBuyBuilding(Player& player);
        void processBankruptcyFlow(Player& payer, Player& owner);
        void processMortgage(Player& player);

    private:
        std::vector<std::unique_ptr<Player>>& players_;
        Board& board_;
        Dice& dice_;
        GameViewInterface& view_;
        CommandInterface& command_;
};
