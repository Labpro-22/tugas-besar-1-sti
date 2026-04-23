#include "models/tile/action_tile/tax_tile/IncomeTaxTile.hpp"

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

OnLandResult IncomeTaxTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    view.showMessage("Kamu mendarat di Pajak Penghasilan (PPH)!\n");
    view.showMessage("Pilih opsi pembayaran pajak:\n");
    view.showMessage("1. Bayar flat M" + std::to_string(getPPHFlatCost()) + "\n");
    view.showMessage("2. Bayar " + std::to_string(static_cast<int>(getTaxPercentage())) + "% dari total kekayaan\n");
    view.showMessage("(Pilih sebelum menghitung kekayaan!)\n");
    view.showMessage("Pilihan (1/2): ");

    int pilihan = command.getInt(1, 2);
    int oldBalance = p.getBalance();
    int taxToPay = 0;

    if (pilihan == 1) {
        taxToPay = getPPHFlatCost();

        if (oldBalance < taxToPay) {
            view.showMessage("Kamu tidak mampu membayar pajak flat M" + std::to_string(taxToPay) + "!\n");
            view.showMessage("Uang kamu saat ini: M" + std::to_string(oldBalance) + "\n");

            int totalWealth = p.getBalance() + p.getTotalPropertyValue() + p.getTotalBuildingValue();

            if (totalWealth < taxToPay) {
                view.showMessage("Jumlah aset + uang kamu juga tidak cukup. Kamu bangkrut ke Bank.\n");
            } else {
                view.showMessage("Jumlah aset + uang kamu masih cukup. Masuk ke alur likuidasi aset.\n");
            }

            view.showMessage("---\n");
            return OnLandResult::TriggerBankruptcyAuction;
        }

        p.deductMoney(taxToPay);
        view.showMessage("Uang kamu: M" + std::to_string(oldBalance) + " -> M" + std::to_string(p.getBalance()) + "\n");
        view.showMessage("---\n");
        return OnLandResult::Done;
    }

    int cash = p.getBalance();
    int propertyVal = p.getTotalPropertyValue();
    int buildingVal = p.getTotalBuildingValue();
    int totalWealth = cash + propertyVal + buildingVal;

    taxToPay = static_cast<int>(totalWealth * getTaxPercentage() * 0.01f);

    view.showMessage("Total kekayaan kamu:\n");
    view.showMessage("- Uang tunai          : M" + std::to_string(cash) + "\n");
    view.showMessage("- Harga beli properti : M" + std::to_string(propertyVal) + " (termasuk yang digadaikan)\n");
    view.showMessage("- Harga beli bangunan : M" + std::to_string(buildingVal) + "\n");
    view.showMessage("Total                 : M" + std::to_string(totalWealth) + "\n");
    view.showMessage("Pajak " + std::to_string(static_cast<int>(getTaxPercentage())) + "%             : M" + std::to_string(taxToPay) + "\n");

    if (oldBalance < taxToPay) {
        view.showMessage("Kamu tidak mampu membayar pajak persentase M" + std::to_string(taxToPay) + "!\n");
        view.showMessage("Uang kamu saat ini: M" + std::to_string(oldBalance) + "\n");

        if (totalWealth < taxToPay) {
            view.showMessage("Jumlah aset + uang kamu juga tidak cukup. Kamu bangkrut ke Bank.\n");
        } else {
            view.showMessage("Jumlah aset + uang kamu masih cukup. Masuk ke alur likuidasi aset.\n");
        }

        view.showMessage("---\n");
        return OnLandResult::TriggerBankruptcyAuction;
    }

    p.deductMoney(taxToPay);
    view.showMessage("Uang kamu: M" + std::to_string(oldBalance) + " -> M" + std::to_string(p.getBalance()) + "\n");
    view.showMessage("---\n");
    return OnLandResult::Done;
}