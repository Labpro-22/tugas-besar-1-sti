#include "controllers/Liquidation.hpp"

#include <algorithm>
#include <sstream>
#include <set>

LiquidationManager::LiquidationManager(GameViewInterface& view, CommandInterface& command) : view_(view), command_(command) {}

std::string LiquidationManager::actionTypeToString(LiquidationActionType type) const {
    if (type == LiquidationActionType::SELL_PROPERTY) return "Jual Properti";
    if (type == LiquidationActionType::MORTGAGE_PROPERTY) return "Gadai Properti";
    return "Jual Bangunan";
}

bool LiquidationManager::containsSelectedAction(const LiquidationAction& action) const {
    return containsAction(selectedActions_, action);
}

bool LiquidationManager::containsAction(const std::vector<LiquidationAction>& actions, const LiquidationAction& target) const {
    for (size_t i = 0; i < actions.size(); i++) {
        if (actions[i].isSameAction(target)) {
            return true;
        }
    }
    return false;
}

bool LiquidationManager::runLiquidation(Player& payer, int obligationAmount) {
    selectedActions_.clear();
    availableActions_.clear();

    int requiredAmount = calculateRequiredAmount(payer, obligationAmount);
    if (requiredAmount <= 0) {
        return true;
    }
    showLiquidationEstimate(payer, obligationAmount);

    int maxLiquidationValue = calculateMaxLiquidationValue(payer);
    if (payer.getBalance() + maxLiquidationValue < obligationAmount) {
        view_.showMessage("Total aset + uang tunai tidak cukup untuk menutup kewajiban.\n");
        return false;
    }
    view_.showMessage("Dana likuidasi dapat menutup kewajiban.\n");
    view_.showMessage("Kamu wajib melikuidasi aset untuk membayar.\n");

    std::vector<LiquidationPlan> plans = buildRecommendedPlans(payer, requiredAmount);
    showRecommendedPlans(plans);

    rebuildAvailableActions(payer);

    while (true) {
        int selectedValue = calculateSelectedValue();
        view_.showMessage("\n=== Panel Likuidasi ===\n");
        view_.showMessage("Uang saat ini     : M" + std::to_string(payer.getBalance()) + "\n");
        view_.showMessage("Kewajiban         : M" + std::to_string(obligationAmount) + "\n");
        view_.showMessage("Kekurangan        : M" + std::to_string(requiredAmount) + "\n");
        view_.showMessage("Nilai terpilih    : M" + std::to_string(selectedValue) + "\n");

        showSelectedActions();

        if (isSelectedValueEnough(requiredAmount)) {
            view_.showMessage("Value sudah mencukupi. Pilih 1 untuk lanjut eksekusi, 0 untuk cancel pilihan.\n");
            int confirm = command_.getInt(0, 1);
            if (confirm == 1) {
                if (confirmFinalSelection()) {
                    executeSelectedActions(payer);
                    return payer.getBalance() >= obligationAmount;
                }
            } else {
                if (!selectedActions_.empty()) {
                    view_.showMessage("Pilih nomor aksi terpilih yang ingin dibatalkan: ");
                    int idx = command_.getInt(1, static_cast<int>(selectedActions_.size()));
                    cancelSelectedAction(idx - 1);
                }
            }
            continue;
        }

        showAvailableActions(availableActions_);

        if (availableActions_.empty()) {
            return false;
        }
        view_.showMessage("Pilih aksi likuidasi (0 untuk batal/cancel pilihan): ");
        int choice = command_.getInt(0, static_cast<int>(availableActions_.size()));
        if (choice == 0) {
            if (selectedActions_.empty()) {
                view_.showMessage("Belum ada aksi yang dipilih.\n");
                continue;
            }
            view_.showMessage("Pilih nomor aksi terpilih yang ingin dibatalkan: ");
            int idx = command_.getInt(1, static_cast<int>(selectedActions_.size()));
            cancelSelectedAction(idx - 1);
            continue;
        }

        LiquidationAction action = availableActions_[choice - 1];

        if (containsSelectedAction(action)) {
            view_.showMessage("Aksi ini sudah dipilih.\n");
            continue;
        }
        if (hasConflictingSelectedAction(action)) {
            view_.showMessage("Aksi konflik: properti ini sudah dipilih untuk aksi likuidasi lain.\n");
            continue;
        }
        if (confirmActionWithPrerequisites(payer, action)) {
            addSelectedAction(action);
        }
    }
}

void LiquidationManager::rebuildAvailableActions(Player& payer) {
    availableActions_.clear();
    std::vector<LiquidationAction> sellActions     = buildSellPropertyActions(payer);
    std::vector<LiquidationAction> mortgageActions = buildMortgageActions(payer);
    std::vector<LiquidationAction> buildingActions = buildSellBuildingActions(payer);
    availableActions_.insert(availableActions_.end(), sellActions.begin(),     sellActions.end());
    availableActions_.insert(availableActions_.end(), mortgageActions.begin(), mortgageActions.end());
    availableActions_.insert(availableActions_.end(), buildingActions.begin(), buildingActions.end());
}

int LiquidationManager::calculateRequiredAmount(Player& payer, int obligationAmount) const {
    int required = obligationAmount - payer.getBalance();
    if (required < 0) return 0;
    return required;
}

int LiquidationManager::calculateSellBuildingValue(StreetTile& street) {
    int level = street.getLevel();
    if (level <= 0) return 0;

    std::map<int, int> buildPrice = street.getBuildPrice();

    {
        std::string dbg = "[DEBUG] calculateSellBuildingValue | tile=" + street.getLetterCode()
                        + " level=" + std::to_string(level)
                        + " buildPrice={";
        for (auto& kv : buildPrice) {
            dbg += std::to_string(kv.first) + ":" + std::to_string(kv.second) + " ";
        }
        dbg += "}\n";
        view_.showMessage(dbg);
    }

    if (buildPrice.empty()) {
        view_.showMessage("[DEBUG] buildPrice KOSONG untuk tile " + street.getLetterCode() + "\n");
        return 0;
    }

    int housePrice = 0;
    int hotelPrice = 0;

    if (buildPrice.size() == 1) {
        housePrice = buildPrice.begin()->second;
        hotelPrice = housePrice;
    } else if (buildPrice.count(1) && buildPrice.count(5)) {
        housePrice = buildPrice.at(1);
        hotelPrice = buildPrice.at(5);
    } else if (buildPrice.count(0) && buildPrice.count(1)) {
        housePrice = buildPrice.at(0);
        hotelPrice = buildPrice.at(1);
    } else if (buildPrice.count(1) && buildPrice.count(2)) {
        housePrice = buildPrice.at(1);
        hotelPrice = buildPrice.at(2);
    } else {
        housePrice = buildPrice.begin()->second;
        hotelPrice = buildPrice.rbegin()->second;
        view_.showMessage("[DEBUG] Menggunakan fallback: housePrice=" +
            std::to_string(housePrice) + " (key=" +
            std::to_string(buildPrice.begin()->first) + "), hotelPrice=" +
            std::to_string(hotelPrice) + " (key=" +
            std::to_string(buildPrice.rbegin()->first) + ")\n");
    }

    int totalCost = (level < 5) ? (level * housePrice) : (4 * housePrice + hotelPrice);

    view_.showMessage("[DEBUG] housePrice=" + std::to_string(housePrice)
        + " hotelPrice=" + std::to_string(hotelPrice)
        + " totalCost=" + std::to_string(totalCost)
        + " -> sellValue=" + std::to_string(totalCost / 2) + "\n");

    return totalCost / 2;
}

int LiquidationManager::calculateMaxLiquidationValue(Player& payer) {
    int total = 0;
    std::vector<PropertyTile*> properties = payer.getProperties();

    view_.showMessage("[DEBUG] jumlah properti payer: " + std::to_string(properties.size()) + "\n");
    for (PropertyTile* p : properties) {
        if (p) view_.showMessage("[DEBUG] properti: " + p->getLetterCode() + 
            " status=" + std::to_string(p->getPropertyStatus()) + "\n");
    }

    std::map<std::string, int> groupBuildingValueCache;
    std::set<std::string> groupBuildingCached;

    for (PropertyTile* property : properties) {
        if (property == nullptr) continue;
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street == nullptr) continue;
        std::string cg = street->getColourBlock();
        if (groupBuildingCached.count(cg)) continue;
        groupBuildingCached.insert(cg);

        int groupVal = 0;
        for (PropertyTile* other : properties) {
            StreetTile* os = dynamic_cast<StreetTile*>(other);
            if (os == nullptr) continue;
            if (os->getColourBlock() != cg) continue;
            if (os->getLevel() <= 0) continue;
            groupVal += calculateSellBuildingValue(*os);
        }
        groupBuildingValueCache[cg] = groupVal;
    }

    std::set<std::string> groupAlreadyEvaluated;

    for (PropertyTile* property : properties) {
        if (property == nullptr) continue;

        StreetTile* street = dynamic_cast<StreetTile*>(property);

        if (street == nullptr) {
            int sellVal     = canSellProperty(payer, *property)     ? calculateSellPropertyValue(*property)  : 0;
            int mortgageVal = canMortgageProperty(payer, *property) ? calculateMortgageValue(*property)      : 0;
            int best = std::max(sellVal, mortgageVal);
            view_.showMessage("[DEBUG] non-street tile=" + property->getLetterCode()
                + " sell=" + std::to_string(sellVal)
                + " mortgage=" + std::to_string(mortgageVal)
                + " best=" + std::to_string(best) + "\n");
            total += best;
            continue;
        }

        std::string cg = street->getColourBlock();

        if (groupAlreadyEvaluated.count(cg)) {
            continue;
        }
        groupAlreadyEvaluated.insert(cg);

        std::vector<StreetTile*> groupTiles;
        for (PropertyTile* other : properties) {
            StreetTile* os = dynamic_cast<StreetTile*>(other);
            if (os != nullptr && os->getColourBlock() == cg) {
                groupTiles.push_back(os);
            }
        }

        int groupBuildVal = groupBuildingValueCache.count(cg) ? groupBuildingValueCache[cg] : 0;
        bool groupHasBuildings = (groupBuildVal > 0);

        int option1 = groupBuildVal;
        for (StreetTile* t : groupTiles) {
            option1 += calculateMortgageValue(*t);
        }

        int option2 = 0;
        for (StreetTile* t : groupTiles) {
            if (canSellProperty(payer, *t)) {
                option2 += calculateSellPropertyValue(*t);
            }
        }

        int option4 = 0;
        if (!groupHasBuildings) {
            for (StreetTile* t : groupTiles) {
                if (canMortgageProperty(payer, *t)) {
                    option4 += calculateMortgageValue(*t);
                }
            }
        }

        int best = std::max({option1, option2, option4});

        view_.showMessage("[DEBUG] colorGroup=" + cg
            + " groupBuildVal=" + std::to_string(groupBuildVal)
            + " option1(sellBldg+mortgageTiles)=" + std::to_string(option1)
            + " option2(sellAllTiles)=" + std::to_string(option2)
            + " option4(mortgageAll)=" + std::to_string(option4)
            + " best=" + std::to_string(best) + "\n");

        total += best;
    }

    view_.showMessage("[DEBUG] calculateMaxLiquidationValue TOTAL=" + std::to_string(total) + "\n");
    return total;
}

std::vector<LiquidationAction> LiquidationManager::buildSellBuildingActions(Player& payer) {
    std::vector<LiquidationAction> actions;
    std::vector<PropertyTile*>     properties = payer.getProperties();

    std::set<std::string> processedGroups;

    for (PropertyTile* property : properties) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street == nullptr) continue;

        std::string colorGroup = street->getColourBlock();
        if (processedGroups.count(colorGroup)) continue;

        int groupBuildingValue = 0;
        StreetTile* representative = nullptr;
        std::string groupDesc;

        for (PropertyTile* other : properties) {
            StreetTile* otherStreet = dynamic_cast<StreetTile*>(other);
            if (otherStreet == nullptr) continue;
            if (otherStreet->getColourBlock() != colorGroup) continue;
            if (otherStreet->getLevel() <= 0) continue;

            int tileVal = calculateSellBuildingValue(*otherStreet);
            groupBuildingValue += tileVal;

            if (representative == nullptr) representative = otherStreet;
            groupDesc += otherStreet->getTileName() + "(" + otherStreet->getLetterCode()
                       + ") M" + std::to_string(tileVal) + " ";
        }

        if (representative == nullptr || groupBuildingValue <= 0) continue;

        processedGroups.insert(colorGroup);

        std::string desc = "Jual semua bangunan di [" + colorGroup + "]: "
                         + groupDesc + "-> Total M" + std::to_string(groupBuildingValue);

        actions.push_back(LiquidationAction(
            LiquidationActionType::SELL_BUILDINGS,
            representative,   
            groupBuildingValue,
            desc
        ));
    }
    return actions;
}

bool LiquidationManager::hasAnyBuildingInOwnedColorGroup(Player& payer, const std::string& colorGroup) {
    std::vector<PropertyTile*> properties = payer.getProperties();
    for (size_t i = 0; i < properties.size(); i++) {
        StreetTile* street = dynamic_cast<StreetTile*>(properties[i]);
        if (street == nullptr) continue;
        if (street->getColourBlock() == colorGroup && street->getLevel() > 0) {
            return true;
        }
    }
    return false;
}

bool LiquidationManager::canMortgageProperty(Player& payer, PropertyTile& property) {
    if (property.getPropertyStatus() != OWNED) {
        return false;
    }
    StreetTile* street = dynamic_cast<StreetTile*>(&property);
    if (street != nullptr && hasAnyBuildingInOwnedColorGroup(payer, street->getColourBlock())) {
        return false;
    }
    return true;
}

bool LiquidationManager::canSellProperty(Player& payer, PropertyTile& property) {
    (void)payer;
    if (property.getPropertyStatus() == MORTGAGED) {
        return false;
    }
    if (property.getPropertyStatus() != OWNED) {
        return false;
    }
    return true;
}

int LiquidationManager::calculateSellPropertyValue(PropertyTile& property) {
    int total = property.getPurchasePrice();
    StreetTile* street = dynamic_cast<StreetTile*>(&property);
    if (street != nullptr && street->getLevel() > 0) {
        total += calculateSellBuildingValue(*street);
    }
    return total;
}

int LiquidationManager::calculateMortgageValue(PropertyTile& property) {
    return property.getMortgageValue();
}

std::vector<LiquidationAction> LiquidationManager::buildSellPropertyActions(Player& payer) {
    std::vector<LiquidationAction> actions;
    std::vector<PropertyTile*> properties = payer.getProperties();
    for (size_t i = 0; i < properties.size(); i++) {
        PropertyTile* property = properties[i];
        if (property == nullptr) continue;
        if (!canSellProperty(payer, *property)) continue;
        int value = calculateSellPropertyValue(*property);
        std::string desc = "Jual " + property->getTileName() + " (" +
            property->getLetterCode() + ") [" + property->getColourBlock() +
            "] -> M" + std::to_string(value);
        actions.push_back(LiquidationAction(
            LiquidationActionType::SELL_PROPERTY,
            property,
            value,
            desc
        ));
    }
    return actions;
}

std::vector<LiquidationAction> LiquidationManager::buildMortgageActions(Player& payer) {
    std::vector<LiquidationAction> actions;
    std::vector<PropertyTile*> properties = payer.getProperties();

    for (size_t i = 0; i < properties.size(); i++) {
        PropertyTile* property = properties[i];
        if (property == nullptr) continue;
        if (!canMortgageProperty(payer, *property)) continue;

        int value = calculateMortgageValue(*property);

        std::string desc = "Gadai " + property->getTileName() + " (" + property->getLetterCode() + ") [" + property->getColourBlock() +"] -> M" + std::to_string(value);
        actions.push_back(LiquidationAction( LiquidationActionType::MORTGAGE_PROPERTY, property, value, desc));
    }
    return actions;
}

std::vector<LiquidationAction> LiquidationManager::findPrerequisitesForAction(Player& payer, LiquidationAction action) {
    std::vector<LiquidationAction> prerequisites;
    PropertyTile* property = action.getProperty();
    if (property == nullptr) return prerequisites;

    if (action.getType() == LiquidationActionType::SELL_BUILDINGS) {
        return prerequisites; 
    }

    if (action.getType() == LiquidationActionType::MORTGAGE_PROPERTY) {
        StreetTile* targetStreet = dynamic_cast<StreetTile*>(property);
        if (targetStreet != nullptr) {
            std::string colorGroup = targetStreet->getColourBlock();
            if (hasAnyBuildingInOwnedColorGroup(payer, colorGroup)) {
                for (const LiquidationAction& avail : availableActions_) {
                    if (avail.getType() == LiquidationActionType::SELL_BUILDINGS) {
                        StreetTile* availStreet = dynamic_cast<StreetTile*>(avail.getProperty());
                        if (availStreet != nullptr && availStreet->getColourBlock() == colorGroup) {
                            if (!containsAction(prerequisites, avail) && !containsSelectedAction(avail)) {
                                prerequisites.push_back(avail);
                            }
                            break;
                        }
                    }
                }
            }
        }
        return prerequisites;
    }
    if (action.getType() == LiquidationActionType::SELL_PROPERTY) {
        StreetTile* targetStreet = dynamic_cast<StreetTile*>(property);
        if (targetStreet != nullptr) {
            std::string colorGroup = targetStreet->getColourBlock();

            bool otherHasBuildings = false;
            std::vector<PropertyTile*> properties = payer.getProperties();
            for (PropertyTile* other : properties) {
                if (other == property) continue; // skip the tile being sold
                StreetTile* otherStreet = dynamic_cast<StreetTile*>(other);
                if (otherStreet == nullptr) continue;
                if (otherStreet->getColourBlock() != colorGroup) continue;
                if (otherStreet->getLevel() > 0) { otherHasBuildings = true; break; }
            }

            if (otherHasBuildings) {
                for (const LiquidationAction& avail : availableActions_) {
                    if (avail.getType() == LiquidationActionType::SELL_BUILDINGS) {
                        StreetTile* availStreet = dynamic_cast<StreetTile*>(avail.getProperty());
                        if (availStreet != nullptr && availStreet->getColourBlock() == colorGroup) {
                            if (!containsAction(prerequisites, avail) && !containsSelectedAction(avail)) {
                                prerequisites.push_back(avail);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    return prerequisites;
}

std::vector<LiquidationPlan> LiquidationManager::buildRecommendedPlans(Player& payer, int requiredAmount) {
    std::vector<LiquidationAction> candidates;

    std::vector<LiquidationAction> sellActions     = buildSellPropertyActions(payer);
    std::vector<LiquidationAction> mortgageActions = buildMortgageActions(payer);
    std::vector<LiquidationAction> buildingActions = buildSellBuildingActions(payer);

    candidates.insert(candidates.end(), sellActions.begin(),     sellActions.end());
    candidates.insert(candidates.end(), mortgageActions.begin(), mortgageActions.end());
    candidates.insert(candidates.end(), buildingActions.begin(), buildingActions.end());

    std::vector<LiquidationPlan> result;
    int n = static_cast<int>(candidates.size());
    int bestValue = -1;

    int totalMasks = 1 << n;
    for (int mask = 1; mask < totalMasks; mask++) {
        LiquidationPlan plan;
        for (int i = 0; i < n; i++) {
            if ((mask & (1 << i)) != 0) {
                plan.addAction(candidates[i]);
            }
        }
        int value = plan.getTotalValue();
        if (value < requiredAmount) continue;
        if (bestValue == -1 || value < bestValue) {
            bestValue = value;
            result.clear();
            result.push_back(plan);
        } else if (value == bestValue) {
            result.push_back(plan);
        }
    }
    return result;
}

void LiquidationManager::showLiquidationEstimate(Player& payer, int obligationAmount) {
    int required = calculateRequiredAmount(payer, obligationAmount);
    int maxValue = calculateMaxLiquidationValue(payer);
    view_.showMessage("Uang kamu       : M" + std::to_string(payer.getBalance()) + "\n");
    view_.showMessage("Total kewajiban : M" + std::to_string(obligationAmount) + "\n");
    view_.showMessage("Kekurangan      : M" + std::to_string(required) + "\n");
    view_.showMessage("Estimasi dana maksimum dari likuidasi: M" + std::to_string(maxValue) + "\n");
}

void LiquidationManager::showRecommendedPlans(const std::vector<LiquidationPlan>& plans) {
    view_.showMessage("\n=== Rekomendasi Likuidasi ===\n");
    if (plans.empty()) {
        view_.showMessage("Tidak ada rekomendasi yang dapat menutup kekurangan.\n");
        return;
    }
    for (size_t i = 0; i < plans.size(); i++) {
        view_.showMessage("Rekomendasi " + std::to_string(i + 1) + " | Total: M" + std::to_string(plans[i].getTotalValue()) + "\n");
        const std::vector<LiquidationAction>& actions = plans[i].getActions();
        for (size_t j = 0; j < actions.size(); j++) {
            view_.showMessage("  - " + actions[j].getDescription() + "\n");
        }
    }
}

void LiquidationManager::showAvailableActions(const std::vector<LiquidationAction>& actions) {
    view_.showMessage("\n=== Aksi Likuidasi Tersedia ===\n");
    for (size_t i = 0; i < actions.size(); i++) {
        view_.showMessage(std::to_string(i + 1) + ". " + actions[i].getDescription() + "\n");
    }
}

void LiquidationManager::showSelectedActions() {
    view_.showMessage("\n=== Aksi Terpilih Sementara ===\n");
    if (selectedActions_.empty()) {
        view_.showMessage("Belum ada aksi dipilih.\n");
        return;
    }
    for (size_t i = 0; i < selectedActions_.size(); i++) {
        view_.showMessage(std::to_string(i + 1) + ". " + selectedActions_[i].getDescription() + "\n");
    }
}

void LiquidationManager::addSelectedAction(const LiquidationAction& action) {
    if (!containsSelectedAction(action)) {
        selectedActions_.push_back(action);
    }
}

void LiquidationManager::cancelSelectedAction(int index) {
    if (index < 0 || index >= static_cast<int>(selectedActions_.size())) {
        return;
    }
    selectedActions_.erase(selectedActions_.begin() + index);
}

int LiquidationManager::calculateSelectedValue() {
    int total = 0;
    for (size_t i = 0; i < selectedActions_.size(); i++) {
        total += selectedActions_[i].getValue();
    }
    return total;
}

bool LiquidationManager::isSelectedValueEnough(int requiredAmount) {
    return calculateSelectedValue() >= requiredAmount;
}

bool LiquidationManager::confirmActionWithPrerequisites(Player& payer, const LiquidationAction& action) {
    std::vector<LiquidationAction> prerequisites = findPrerequisitesForAction(payer, action);
    if (!prerequisites.empty()) {
        view_.showMessage("Aksi ini membutuhkan prerequisite:\n");
        for (size_t i = 0; i < prerequisites.size(); i++) {
            view_.showMessage("- " + prerequisites[i].getDescription() + "\n");
        }
        view_.showMessage("Setujui prerequisite? 1 = ya, 0 = cancel\n");
        int ok = command_.getInt(0, 1);
        if (ok == 0) {
            return false;
        }
        for (size_t i = 0; i < prerequisites.size(); i++) {
            addSelectedAction(prerequisites[i]);
        }
    }
    view_.showMessage("Tambahkan aksi ini? 1 = ya, 0 = cancel\n");
    int confirm = command_.getInt(0, 1);
    return confirm == 1;
}

bool LiquidationManager::confirmFinalSelection() {
    view_.showMessage("Value sudah mencukupi. Lanjutkan ke akhir proses likuidasi? 1 = ya, 0 = cancel\n");
    int confirm = command_.getInt(0, 1);
    return confirm == 1;
}

void LiquidationManager::executeSelectedActions(Player& payer) {
    sortSelectedActionsByPriority();
    for (size_t i = 0; i < selectedActions_.size(); i++) {
        LiquidationAction action = selectedActions_[i];
        PropertyTile* property = action.getProperty();
        if (property == nullptr) continue;
        if (action.getType() == LiquidationActionType::SELL_PROPERTY) {
            executeSellProperty(payer, *property);
        } else if (action.getType() == LiquidationActionType::MORTGAGE_PROPERTY) {
            executeMortgageProperty(payer, *property);
        } else if (action.getType() == LiquidationActionType::SELL_BUILDINGS) {
            StreetTile* street = dynamic_cast<StreetTile*>(property);
            if (street != nullptr) {
                executeSellBuildingsColorGroup(payer, street->getColourBlock());
            }
        }
    }
    selectedActions_.clear();
}

void LiquidationManager::executeSellProperty(Player& payer, PropertyTile& property) {
    int value = calculateSellPropertyValue(property);

    payer.addMoney(value);
    payer.removeProperty(&property);

    property.setOwnerUsername("BANK");
    property.setPropertyStatus(BANK);
    property.setFestivalMultiplier(1);
    property.setFestivalDuration(0);
    property.setLevel(0);

    view_.showMessage(property.getTileName() + " (" + property.getLetterCode() + ") terjual ke Bank. Kamu menerima M" + std::to_string(value) + "\n");
}

void LiquidationManager::executeMortgageProperty(Player& payer, PropertyTile& property) {
    int value = calculateMortgageValue(property);
    payer.addMoney(value);
    property.setPropertyStatus(MORTGAGED);
    view_.showMessage(property.getTileName() + " (" + property.getLetterCode() + ") berhasil digadaikan. Kamu menerima M" + std::to_string(value) + "\n");
}

void LiquidationManager::executeSellBuildings(Player& payer, StreetTile& street) {
    int value = calculateSellBuildingValue(street);
    payer.addMoney(value);
    street.setLevel(0);
    view_.showMessage("Semua bangunan di " + street.getTileName() + " (" + street.getLetterCode() + ") terjual. Kamu menerima M" + std::to_string(value) + "\n");
}

void LiquidationManager::executeSellBuildingsColorGroup(Player& payer, const std::string& colorGroup) {
    std::vector<PropertyTile*> properties = payer.getProperties();
    for (PropertyTile* property : properties) {
        StreetTile* street = dynamic_cast<StreetTile*>(property);
        if (street == nullptr) continue;
        if (street->getColourBlock() != colorGroup) continue;
        if (street->getLevel() <= 0) continue;
        executeSellBuildings(payer, *street);
    }
}

void LiquidationManager::sortSelectedActionsByPriority() {
    std::sort(selectedActions_.begin(), selectedActions_.end(),
        [](const LiquidationAction& a, const LiquidationAction& b) {
            if (a.getType() == LiquidationActionType::SELL_BUILDINGS &&
                b.getType() != LiquidationActionType::SELL_BUILDINGS)
                return true;
            if (b.getType() == LiquidationActionType::SELL_BUILDINGS &&
                a.getType() != LiquidationActionType::SELL_BUILDINGS)
                return false;
            return false;
        });
}

bool LiquidationManager::hasConflictingSelectedAction(const LiquidationAction& action) const {
    PropertyTile* target = action.getProperty();
    if (target == nullptr) return false;
    for (const LiquidationAction& selected : selectedActions_) {
        PropertyTile* selectedProperty = selected.getProperty();
        if (selectedProperty == nullptr) continue;

        if (selectedProperty->getTileID() == target->getTileID()) {
            if (selected.getType() != action.getType()) {
                return true;
            }
        }
    }
    return false;
}
