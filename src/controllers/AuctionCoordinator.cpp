#include "controllers/AuctionCoordinator.hpp"

AuctionCoordinator::AuctionCoordinator(std::vector<std::unique_ptr<Player>>& players,
                                    GameViewInterface& view,
                                    CommandInterface& command)
    : players_(players), view_(view), command_(command) {}

void AuctionCoordinator::processAuction(Player& triggerPlayer, PropertyTile& propertyTile) {
    Auction auction(players_, view_);
    auction.runAuction(triggerPlayer, propertyTile, Auction::AuctionCause::PROPERTY_DECLINED, command_);
}

void AuctionCoordinator::processBankruptcyToBank(Player& player) {
    Auction auctionEngine(players_, view_);
    auctionEngine.runBankruptcyAuction(player, command_);
}