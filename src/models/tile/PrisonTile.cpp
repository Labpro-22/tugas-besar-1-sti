#include "models/tile/PrisonTile.hpp"
#include "models/player/Player.hpp"
#include "models/monopoly/Monopoly.hpp"
#include <iostream>
#include <limits>

void PrisonTile::executeTile(Player* p) {
    std::cout << "Kamu mendarat di Petak Penjara!\n";
    std::cout << "Kamu dapat melanjutkan perjalanan di giliran selanjutnya.\n";
    std::cout << "---\n";
}

int PrisonTile::tryExitPrison(Player *p) {
    // TODO: implementasi fungsi di Player
    p->incrementJailTurn();
    int turns = p->getCountJail();
    int oldBalance = p->getBalance();
    
    std::cout << "Status Tahanan: Giliran ke-" << turns << "\n";

    if (turns > 3) {
        std::cout << "Batas waktu habis! Kamu wajib membayar denda M" << getFineCost() << ".\n";
        
        if (p->getBalance() < getFineCost()) {
            std::cout << "Uangmu tidak cukup (M" << p->getBalance() << "). Kamu bangkrut!\n";
            // TODO: Recheck di Exception
            throw InsufficientFundsException();
        }
        
        p->deductMoney(getFineCost());
        p->setStatus(ACTIVE);
        // TODO: implementasi fungsi di Player
        p->resetJailTurn();
        std::cout << "Kamu membayar denda M" << getFineCost() << " dan bebas!\n";
        std::cout << "Uang kamu: M" << oldBalance << " -> M" << p->getBalance() << "\n";
        return 0;
    }

    bool punyaKartu = false;

    std::cout << "Pilih cara untuk keluar:\n";
    std::cout << "1. Bayar denda M" << getFineCost() << "\n";
    // if (punyaKartu) std::cout << "2. Gunakan kartu Bebas Penjara\n";
    std::cout << "2. Kocok dadu (Harus Double)\n";

    int pilihan;
    while (true) {
        std::cout << "Pilihan: ";
        if (std::cin >> pilihan && (pilihan == 1 || pilihan == 2)) {
            break;
        }
        std::cout << "Input tidak valid! Pilih angka 1/2.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cout << "\n";

    if (pilihan == 1) {
        if (p->getBalance() < getFineCost()) {
            std::cout << "Uangmu tidak cukup (M" << p->getBalance() << "). Kamu bangkrut!\n";
            throw InsufficientFundsException();
        }
        p->deductMoney(getFineCost());
        p->setStatus(ACTIVE);
        p->resetJailTurn();
        std::cout << "Kamu membayar denda M" << getFineCost() << " dan bebas!\n";
        std::cout << "Uang kamu: M" << oldBalance << " -> M" << p->getBalance() << "\n";
        return 0;

    // } else if (pilihan == 2) {
    //     p->getInventory().useJailFreeCard();
    //     p->setStatus(ACTIVE);
    //     p->resetJailTurn();
    //     std::cout << "Kamu menggunakan Kartu Bebas Penjara dan terbebas!\n";
    //     return 0;

    } else {
        Monopoly& m = p->getMonopoly(); 
        m.getDice()[0].rollRandom();
        m.getDice()[1].rollRandom();
        int v1 = m.getDice()[0].getFaceValue();
        int v2 = m.getDice()[1].getFaceValue();
        int hasilDadu = v1 + v2; 
        
        std::cout << "Kamu mengocok dadu: " << v1 << " dan " << v2 << "\n";
        
        if (v1 == v2) {
            std::cout << "DOUBLE! Kamu terbebas dari penjara!\n";
            p->setStatus(ACTIVE);
            p->resetJailTurn();
            
            // TODO: Confirm penanganan di takeTurn
            return hasilDadu; 
        } else {
            std::cout << "Gagal mendapatkan angka double. Kamu tetap di penjara giliran ini.\n";

            return 0;
        }
    }

    return 0;
}