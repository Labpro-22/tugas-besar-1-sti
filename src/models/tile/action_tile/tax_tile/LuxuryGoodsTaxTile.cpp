#include "models/tile/action_tile/tax_tile/LuxuryGoodsTaxTile.hpp"

void LuxuryGoodsTaxTile::setPBMFlatCost(int cost) {
    flatCost_ = cost;
}

int LuxuryGoodsTaxTile::getPBMFlatCost() const {
    return flatCost_;
}

OnLandResult LuxuryGoodsTaxTile::onLand(Player& p, CommandInterface& command, GameViewInterface& view) {
    (void)command;
    (void)view;
    std::cout << "Kamu mendarat di Pajak Barang Mewah (PBM)!\n";
    
    int taxToPay = getPBMFlatCost();
    int oldBalance = p.getBalance();
    
    std::cout << "Pajak sebesar M" << taxToPay << " langsung dipotong.\n";
    
    if (oldBalance < taxToPay) {
        std::cout << "Kamu tidak mampu membayar pajak!\n";
        std::cout << "Uang kamu saat ini: M" << oldBalance << "\n";
        
        // TODO: recheck parameter
        throw PBMPaymentFailed();
    } else {
        p.deductMoney(taxToPay);
        std::cout << "Uang kamu: M" << oldBalance << " -> M" << p.getBalance() << "\n";
    }
    std::cout << "---\n";
    return OnLandResult::Done;
}