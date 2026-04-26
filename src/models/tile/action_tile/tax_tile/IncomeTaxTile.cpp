#include "models/tile/action_tile/tax_tile/IncomeTaxTile.hpp"
#include "views/GameViewInterface.hpp"
#include "views/CommandInterface.hpp"
#include "models/player/Player.hpp"
#include "utils/Formatter.hpp"

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
    view.showMessage("1. Bayar flat " + Formatter::formattingMoney(getPPHFlatCost()) + "\n");
    view.showMessage("2. Bayar " + std::to_string(static_cast<int>(getTaxPercentage())) + "% dari total kekayaan\n");
    view.showMessage("(Pilih sebelum menghitung kekayaan!)\n");
    
    int pilihan = command.getInt(1, 2); 
    int oldBalance = p.getBalance();
    int taxToPay = 0;

    if (pilihan == 1) {
        taxToPay = getPPHFlatCost();

        if (oldBalance < taxToPay) {
            view.showMessage("\nKamu tidak mampu membayar pajak flat " + Formatter::formattingMoney(taxToPay) + "!\n");
            view.showMessage("Uang kamu saat ini: " + Formatter::formattingMoney(oldBalance) + "\n");
            return OnLandResult::TriggerBankruptcyAuction;
        }

        p.deductMoney(taxToPay);
        view.showMessage("\nUang kamu: " + Formatter::formattingMoney(oldBalance) + " -> " + Formatter::formattingMoney(p.getBalance()) + "\n");
        view.showMessage("---\n");
        return OnLandResult::Done;
    }

    int cash = p.getBalance();
    int propertyVal = p.getTotalPropertyValue();
    int buildingVal = p.getTotalBuildingValue();
    int totalWealth = cash + propertyVal + buildingVal;
    int pct = static_cast<int>(getTaxPercentage());

    taxToPay = static_cast<int>(totalWealth * pct * 0.01f);

    auto padR = [](std::string s) {
        const int width = 22;
        if (s.length() >= width) return s;
        return s + std::string(width - s.length(), ' ');
    };

    view.showMessage("\nTotal kekayaan kamu:\n");
    view.showMessage(padR("- Uang tunai") + ": " + Formatter::formattingMoney(cash) + "\n");
    view.showMessage(padR("- Harga beli properti") + ": " + Formatter::formattingMoney(propertyVal) + " (termasuk yang digadaikan)\n");
    view.showMessage(padR("- Harga beli bangunan") + ": " + Formatter::formattingMoney(buildingVal) + "\n");
    view.showMessage(padR("Total") + ": " + Formatter::formattingMoney(totalWealth) + "\n");
    view.showMessage(padR("Pajak " + std::to_string(pct) + "%") + ": " + Formatter::formattingMoney(taxToPay) + "\n");

    if (oldBalance < taxToPay) {
        view.showMessage("\nKamu tidak mampu membayar pajak persentase " + Formatter::formattingMoney(taxToPay) + "!\n");
        view.showMessage("Uang kamu saat ini: " + Formatter::formattingMoney(oldBalance) + "\n");
        return OnLandResult::TriggerBankruptcyAuction;
    }

    p.deductMoney(taxToPay);
    view.showMessage("Uang kamu: " + Formatter::formattingMoney(oldBalance) + " -> " + Formatter::formattingMoney(p.getBalance()) + "\n");
    view.showMessage("---\n");
    
    return OnLandResult::Done;
}