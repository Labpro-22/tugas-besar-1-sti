#include "models/tile/action_tile/tax_tile/LuxuryGoodsTaxTile.hpp"
#include "views/GameViewInterface.hpp"
#include "views/CommandInterface.hpp"
#include "models/player/Player.hpp"

void LuxuryGoodsTaxTile::setPBMFlatCost(int cost) {
    flatCost_ = cost;
}

int LuxuryGoodsTaxTile::getPBMFlatCost() const {
    return flatCost_;
}

OnLandResult LuxuryGoodsTaxTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    (void) command;

    view.showMessage("Kamu mendarat di Pajak Barang Mewah (PBM)!\n");

    int taxToPay = getPBMFlatCost();
    int oldBalance = p.getBalance();

    view.showMessage("Pajak sebesar M" + std::to_string(taxToPay) + " langsung dipotong.\n");

    if (oldBalance < taxToPay) {
        view.showMessage("Kamu tidak mampu membayar pajak!\n");
        view.showMessage("Uang kamu saat ini: M" + std::to_string(oldBalance) + "\n");

        int totalWealth = p.getBalance() + p.getTotalPropertyValue() + p.getTotalBuildingValue();

        view.showMessage("Total kewajiban : M" + std::to_string(taxToPay) + "\n");
        view.showMessage("Total aset + uang tunai : M" + std::to_string(totalWealth) + "\n");

        if (totalWealth < taxToPay) {
            view.showMessage("Tidak cukup untuk menutup kewajiban. Kamu bangkrut ke Bank.\n");
        } else {
            view.showMessage("Jumlah aset + uang masih cukup. Masuk ke alur likuidasi aset.\n");
        }

        view.showMessage("---\n");
        return OnLandResult::TriggerBankruptcyAuction;
    }

    p.deductMoney(taxToPay);
    view.showMessage("Uang kamu: M" + std::to_string(oldBalance) +
                     " -> M" + std::to_string(p.getBalance()) + "\n");
    view.showMessage("---\n");

    return OnLandResult::Done;
}