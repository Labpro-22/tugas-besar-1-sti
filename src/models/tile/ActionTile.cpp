#include "../../../include/models/tile/ActionTile.hpp"
#include "../../../include/models/player/Player.hpp"
#include "../../../include/models/exception/Exception.hpp"
#include <iostream>
#include <string>
#include <limits>

void CommunityChestTile::executeTile(Player* p) {
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

void ChanceTile::executeTile(Player* p) {
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
        bool isCodeExistOnBoard = p->getBoard()->isPropertyExist(inputCode);

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


// Deklarasi nilai awal biar ga aneh-aneh
int IncomeTaxTile::flatCost_ = 0;
float IncomeTaxTile::taxPercentage_ = 0;
int LuxuryGoodsTaxTile::flatCost_ = 0;

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
    std::cout << "1. Bayar flat M" << getFlatCost() << "\n";
    std::cout << "2. Bayar " << getTaxPercentage() << "% dari total kekayaan\n";
    std::cout << "(Pilih sebelum menghitung kekayaan!)\n";
    
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
        taxToPay = getFlatCost();
        
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

void LuxuryGoodsTaxTile::setPBMFlatCost(int cost) {
    flatCost_ = cost;
}

int LuxuryGoodsTaxTile::getPBMFlatCost() const {
    return flatCost_;
}

void LuxuryGoodsTaxTile::executeTile(Player* p) {
    std::cout << "Kamu mendarat di Pajak Barang Mewah (PBM)!\n";
    
    int taxToPay = getFlatCost();
    int oldBalance = p->getBalance();
    
    std::cout << "Pajak sebesar M" << taxToPay << " langsung dipotong.\n";
    
    if (oldBalance < taxToPay) {
        std::cout << "Kamu tidak mampu membayar pajak!\n";
        std::cout << "Uang kamu saat ini: M" << oldBalance << "\n";
        
        // TODO: recheck parameter
        throw PBMPaymentFailed();
    } else {
        p->deductMoney(taxToPay);
        std::cout << "Uang kamu: M" << oldBalance << " -> M" << p->getBalance() << "\n";
    }
    std::cout << "---\n";
}