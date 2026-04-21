#include "models/tile/action_tile/special_tile/GoToJailTile.hpp"
#include "models/player/Player.hpp"
#include <iostream>

void GoToJailTile::onLand(Player& p) {
    std::cout << "Kamu mendarat di petak Pergi Ke Penjara!\n";
    std::cout << "Polisi menangkapmu! Bidakmu langsung dipindahkan ke Penjara.\n";

    p->setStatus(JAILED);

    // TODO: implement fungsi di Board
    int posisiPenjara = p->getBoard().getJailPosition();
    // TODO: di fungsinya harus ngecek status Player dulu, biar ga dikasih salary
    p->setPosition(posisiPenjara);
    
    std::cout << "---\n";
}