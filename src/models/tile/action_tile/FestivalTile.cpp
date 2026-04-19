#include "models/tile/action_tile/FestivalTile.hpp"
#include "models/player/Player.hpp"
#include "models/exception/Exception.hpp"
#include <iostream>
#include <string>
#include <limits>

void FestivalTile::executeTile(Player* p) {
    std::cout << "Kamu mendarat di petak Festival!\n\n";

    Inventory& inv = p->getInventory();
    
    // TODO: Perbarui cara mengecek jumlah properti Player
    if (inv.getPropertyCount() == 0) {
        std::cout << "Kamu belum memiliki properti apa pun untuk mengadakan festival.\n";
        std::cout << "---\n";
        return;
    }

    std::cout << "Daftar properti milikmu:\n";
    for (int i = 0; i < inv.getPropertyCount(); i++) {
        PropertyTile* prop = inv.getProperty(i);
        std::cout << "- " << prop->getLetterCode() << " (" << prop->getTileName() << ")\n";
    }
    std::cout << "\n";

    std::string inputCode;
    PropertyTile* chosenProp = nullptr;

    while (true) {
        std::cout << "Masukkan kode properti: ";
        std::cin >> inputCode;

        // TODO: recheck fungsi
        chosenProp = inv.getPropertyByCode(inputCode);

        if (chosenProp != nullptr) {
            break; 
        }

        // TODO: Recheck getter Board
        bool isCodeExistOnBoard = p->getBoard().isPropertyExist(inputCode);

        if (isCodeExistOnBoard) {
            std::cout << "-> Properti bukan milikmu!\n\n";
        } else {
            std::cout << "-> Kode properti tidak valid!\n\n";
        }
        
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    int oldRent = chosenProp->getCurrentRent(); 
    
    bool isUpgraded = chosenProp->applyFestival();

    if (isUpgraded) {
        if (chosenProp->getFestivalMultiplier() == 2) {
            std::cout << "\nEfek festival aktif!\n\n";
            std::cout << "Sewa awal: M" << oldRent << "\n";
        } else {
            std::cout << "\nEfek diperkuat!\n\n";
            std::cout << "Sewa sebelumnya: M" << oldRent << "\n";
        }
        std::cout << "Sewa sekarang: M" << chosenProp->getCurrentRent() << "\n";
        std::cout << "Durasi: 3 giliran\n";
        
    } else {
        std::cout << "\nEfek sudah maksimum (harga sewa sudah digandakan tiga kali)\n\n";
        std::cout << "Durasi di-reset menjadi: 3 giliran\n";
    }

    std::cout << "---\n";
}