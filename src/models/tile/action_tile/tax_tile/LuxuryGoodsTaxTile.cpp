#include "models/tile/action_tile/tax_tile/LuxuryGoodsTaxTile.hpp"
#include "views/GameViewInterface.hpp"
#include "views/CommandInterface.hpp"
#include "utils/Formatter.hpp"
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

    view.showMessage("Pajak sebesar " + Formatter::formattingMoney(taxToPay) + " langsung dipotong.\n");

    if (oldBalance < taxToPay) {
        view.showMessage("Kamu tidak mampu membayar pajak!\n");
        view.showMessage("Uang kamu saat ini: " + Formatter::formattingMoney(oldBalance) + "\n");
        return OnLandResult::TriggerBankruptcyAuction;
    }

    p.deductMoney(taxToPay);
    view.showMessage("Uang kamu: " + Formatter::formattingMoney(oldBalance) + " -> " + Formatter::formattingMoney(p.getBalance()) + "\n");
    view.showMessage("---\n");

    return OnLandResult::Done;
}