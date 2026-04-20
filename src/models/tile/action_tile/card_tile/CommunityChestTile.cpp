#include "models/tile/action_tile/card_tile/CommunityChestTile.hpp"
#include "models/player/Player.hpp"
#include "models/exception/Exception.hpp"
#include <iostream>
#include <string>
#include <limits>

void CommunityChestTile::onLand(Player& p){
    std::cout << "Kamu mendarat di Petak Dana Umum!\n";
    std::cout << "Mengambil kartu...\n";

    // TODO: Recheck getter di Board
    Deck<CommunityChestCard>& deck = p->getBoard().getCommunityChestDeck();
    
    CommunityChestCard* drawnCard = deck.drawDeck();

    // TODO: Recheck getter di ChanceCard
    std::cout << "Kartu: \"" << drawnCard->getDescription() << "\"\n";

    drawnCard->useCommunityChest(p);

    // TODO: Recheck metode di Deck
    deck.returnAndReshuffle(drawnCard);

    std::cout << "---\n";
}