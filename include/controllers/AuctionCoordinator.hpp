#pragma once

#include "models/player/Player.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include "views/CommandInterface.hpp"
#include "views/GameViewInterface.hpp"
#include "controllers/Auction.hpp"
#include <memory>
#include <vector>

class AuctionCoordinator {
    public:
        AuctionCoordinator(std::vector<std::unique_ptr<Player>>& players,
                        GameViewInterface& view,
                        CommandInterface& command);

        void transferProperty(Player& from, Player& to, PropertyTile& propertyTile);
        void processAuction(Player& triggerPlayer, PropertyTile& propertyTile);
        void processBankruptcyToBank(Player& player);

    private:
        std::vector<std::unique_ptr<Player>>& players_;
        GameViewInterface& view_;
        CommandInterface& command_;
};
