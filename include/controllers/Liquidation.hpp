#pragma once

#include <vector>
#include <string>
#include <map>

#include "models/player/Player.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/tile/property_tile/StreetTile.hpp"
#include "views/GameViewInterface.hpp"
#include "views/CommandInterface.hpp"

enum class LiquidationActionType {
    SELL_PROPERTY,
    MORTGAGE_PROPERTY,
    SELL_BUILDINGS
};

// pasangan property, aksi yang bisa dilakuin (LiquidationActionType), dan value yang didapet dari aksi itu (harga jual atau nilai gadai)
class LiquidationAction {
private:
    LiquidationActionType type_;
    PropertyTile* property_;
    int value_;
    std::string description_;

public:
    LiquidationAction() : type_(LiquidationActionType::SELL_PROPERTY), property_(nullptr), value_(0), description_("") {}

    LiquidationAction(LiquidationActionType type, PropertyTile* property, int value, const std::string& description) : type_(type), property_(property), value_(value), description_(description) {}

    LiquidationActionType getType() const {return type_;}

    PropertyTile* getProperty() const {return property_; }

    int getValue() const {return value_;}

    std::string getDescription() const {return description_;}

    bool isSameAction(const LiquidationAction& other) const {return type_ == other.type_ && property_ == other.property_;}
};

// buat tentuin pasangan" aksinya (total value bisa bayar rent)
class LiquidationPlan {
private:
    std::vector<LiquidationAction> actions_;
    int totalValue_;

public:
    LiquidationPlan::LiquidationPlan() : totalValue_(0) {}

    void LiquidationPlan::addAction(const LiquidationAction& action) {
        actions_.push_back(action);
        totalValue_ += action.getValue();
    }

    const std::vector<LiquidationAction>& LiquidationPlan::getActions() const {return actions_;}

    int LiquidationPlan::getTotalValue() const {return totalValue_;}
};

// action likuidasinya
class LiquidationManager {
private:
    GameViewInterface& view_;
    CommandInterface& command_;

    std::vector<LiquidationAction> availableActions_;
    std::vector<LiquidationAction> selectedActions_;

    std::string actionTypeToString(LiquidationActionType type) const;
    bool containsSelectedAction(const LiquidationAction& action) const;
    bool containsAction(const std::vector<LiquidationAction>& actions, const LiquidationAction& target) const;

public:
    LiquidationManager(GameViewInterface& view, CommandInterface& command);

    bool runLiquidation(Player& payer, int obligationAmount); // panggil ini di controller

    // hitung kebutuhan likuidasi
    int calculateRequiredAmount(Player& payer, int obligationAmount) const;
    int calculateMaxLiquidationValue(Player& payer);
    
    // kumpulin aksi yang bisa dilakuin
    std::vector<LiquidationAction> buildSellPropertyActions(Player& payer);
    std::vector<LiquidationAction> buildMortgageActions(Player& payer);
    std::vector<LiquidationAction> buildSellBuildingActions(Player& payer);

    // cek cek
    bool hasAnyBuildingInOwnedColorGroup(Player& payer, const std::string& colorGroup);
    bool canMortgageProperty(Player& payer, PropertyTile& property);
    bool canSellProperty(Player& payer, PropertyTile& property);

    // itung value dari aksi
    int calculateSellPropertyValue(PropertyTile& property);
    int calculateMortgageValue(PropertyTile& property);
    int calculateSellBuildingValue(StreetTile& street);

    // cari prerequisite buat aksi
    std::vector<LiquidationAction> findPrerequisitesForAction(Player& payer, LiquidationAction action);
    std::vector<LiquidationPlan> buildRecommendedPlans(Player& payer, int requiredAmount);

    // cli nya
    void showLiquidationEstimate(Player& payer, int obligationAmount);
    void showRecommendedPlans(const std::vector<LiquidationPlan>& plans);
    void showAvailableActions(const std::vector<LiquidationAction>& actions);
    void showSelectedActions();
    
    // add and calcel aksi yang dipilih
    void addSelectedAction(const LiquidationAction& action);
    void cancelSelectedAction(int index);
    
    // hitung total value dari aksi yang dipilih
    int calculateSelectedValue();
    bool isSelectedValueEnough(int requiredAmount);

    // konfirmasi ke user apakah mau lakuin aksi yang dipilih, dengan prerequisite yang harus dilakuin juga
    bool confirmActionWithPrerequisites(Player& payer, const LiquidationAction& action);
    bool confirmFinalSelection();

    // eksekusi aksi yang dipilih
    void executeSelectedActions(Player& payer);
    void executeSellProperty(Player& payer, PropertyTile& property);
    void executeMortgageProperty(Player& payer, PropertyTile& property);
    void executeSellBuildings(Player& payer, StreetTile& street);

    void sortSelectedActionsByPriority();
};