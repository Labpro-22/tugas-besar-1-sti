#include "models/tile/action_tile/tax_tile/IncomeTaxTile.hpp"
#include "models/player/Player.hpp"
#include "models/exception/Exception.hpp"
#include <iostream>
#include <string>
#include <limits>

void IncomeTaxTile::setPPHFlatCost(int cost) {
    flatCost_ = cost;
}

void IncomeTaxTile::setTaxPercentage(float percentage) {
    taxPercentage_ = percentage;
}

int IncomeTaxTile::getPPHFlatCost() const {
    return flatCost_;
}

float IncomeTaxTile::getTaxPercentage() const {
    return taxPercentage_;
}

void IncomeTaxTile::executeTile(Player* p) {
    std::cout << "Kamu mendarat di Pajak Penghasilan (PPH)!\n";
    std::cout << "Pilih opsi pembayaran pajak:\n";
    std::cout << "1. Bayar flat M" << getPPHFlatCost() << "\n";
    std::cout << "2. Bayar " << getTaxPercentage() << "% dari total kekayaan\n";
    std::cout << "(Pilih sebelum menghitung kekayaan!)\n";
    int pilihan = 0;
    
    while (true) {
        std::cout << "Pilihan (1/2): ";
        if (std::cin >> pilihan && (pilihan == 1 || pilihan == 2)) {
            break;
        }
        std::cout << "Input tidak valid! Harap hanya masukkan angka 1 atau 2.\n";
        std::cin.clear(); 
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }

    int oldBalance = p->getBalance();
    int taxToPay = 0;

    if (pilihan == 1) {
        taxToPay = getPPHFlatCost();
        
        if (oldBalance < taxToPay) {
            std::cout << "Kamu tidak mampu membayar pajak flat M" << taxToPay << "!\n";
            std::cout << "Uang kamu saat ini: M" << oldBalance << "\n";
            
            // TODO: recheck parameter
            throw PPHPaymentFailed();
        } else {
            p->deductMoney(taxToPay);
            std::cout << "Uang kamu: M" << oldBalance << " -> M" << p->getBalance() << "\n";
        }
        
    } else {
        int cash = p->getBalance();
        // TODO: Buat getter properti di Player
        int propertyVal = p->getTotalPropertyValue();
        int buildingVal = p->getTotalBuildingValue();
        int totalWealth = cash + propertyVal + buildingVal;
        
        taxToPay = static_cast<int>(totalWealth * getTaxPercentage() * 0.01);

        std::cout << "Total kekayaan kamu:\n";
        std::cout << "- Uang tunai          : M" << cash << "\n";
        std::cout << "- Harga beli properti : M" << propertyVal << " (termasuk yang digadaikan)\n";
        std::cout << "- Harga beli bangunan : M" << buildingVal << "\n";
        std::cout << "Total                 : M" << totalWealth << "\n";
        std::cout << "Pajak " << getTaxPercentage() << "%             : M" << taxToPay << "\n";

        if (oldBalance < taxToPay) {
            std::cout << "Kamu tidak mampu membayar pajak persentase M" << taxToPay << "!\n";
            std::cout << "Uang kamu saat ini: M" << oldBalance << "\n";
            
            // TODO: recheck parameter
            throw PBMPaymentFailed();
        } else {
            p->deductMoney(taxToPay);
            std::cout << "Uang kamu: M" << oldBalance << " -> M" << p->getBalance() << "\n";
        }
    }
    std::cout << "---\n";
}