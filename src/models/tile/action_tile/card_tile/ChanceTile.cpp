#include "models/tile/action_tile/card_tile/ChanceTile.hpp"
#include "models/player/Player.hpp"
#include "models/exception/Exception.hpp"
#include <iostream>
#include <string>
#include <limits>

void ChanceTile::onLand(Player& p) {
    std::cout << "Kamu mendarat di Petak Kesempatan!\n";
    std::cout << "Mengambil kartu...\n";

    // TODO: Recheck getter di Board
    Deck<ChanceCard>& deck = p->getBoard().getChanceDeck();

    ChanceCard* drawnCard = deck.drawDeck();

    // TODO: Recheck getter di ChanceCard
    std::cout << "Kartu: \"" << drawnCard->getName() << "\"\n";

    drawnCard->useChance(p);

    // TODO: Recheck metode di Deck
    deck.returnAndReshuffle(drawnCard);

    std::cout << "---\n";
}