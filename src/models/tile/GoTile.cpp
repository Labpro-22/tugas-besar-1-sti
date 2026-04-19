#include "models/tile/GoTile.hpp"
#include "models/player/Player.hpp"
#include <iostream>

void GoTile::executeTile(Player* p) {
    // TODO: Kalau melewati doang tuh dihandle Player
    std::cout << "Kamu mendarat di Petak Go!\n";
    std::cout << "Menerima bonus gaji sebesar M" << getSalary() << "\n";
    
    int oldBalance = p->getBalance();
    p->addMoney(getSalary());

    std::cout << "Uang kamu: M" << oldBalance << " -> M" << p->getBalance() << "\n";
    std::cout << "---\n";
}