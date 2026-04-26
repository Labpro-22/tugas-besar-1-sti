#include "controllers/PropertyCoordinator.hpp"

#include <iostream>
#include <iterator>
#include <map>

PropertyCoordinator::PropertyCoordinator(std::vector<std::unique_ptr<Player>>& players,
                                        Board& board,
                                        Dice& dice,
                                        GameViewInterface& view,
                                        CommandInterface& command)
    : players_(players), board_(board), dice_(dice), view_(view), command_(command) {}

void PropertyCoordinator::processFestival(Player& player) {
    view_.showMessage("Daftar properti milikmu...\n");

    std::string chosenPropCode;
    PropertyTile* propTile = nullptr;
    do {
        view_.showMessage("Masukkan kode properti untuk festival: ");
        Command cmd = command_.getCommand();
        chosenPropCode = cmd.getStringArg();
        if (!board_.has(chosenPropCode)) {
            view_.showMessage("Kode tidak valid!\n");
            continue;
        }

        if (!player.hasProperty(chosenPropCode)) {
            view_.showMessage("Tetot\n");
            continue;
        }

        propTile = &player.getProperty(chosenPropCode);

        if (!propTile->canDoubleFestival()) {
            view_.showMessage("Ga bs double lagi woyyy\n");
            propTile = nullptr;
            continue;
        }

    } while (propTile == nullptr);

    if (propTile->festivalActive()) {
        if (propTile->canDoubleFestival()) {
            if (!propTile->alreadyMaxMultiplier()) {
                propTile->doubleTheMultiplier();
            }
            propTile->resetTurnTo3();
        }
    } else {
        propTile->doubleTheMultiplier();
        propTile->resetTurnTo3();
    }

    view_.showMessage("Hihi hiha!\n");
}

void PropertyCoordinator::processTakeChanceCard(Player& player) {
    ChanceCard card = ChanceCard::randomCard();
    view_.showMessage("[CHANCE] " + card.getDescription() + "\n");

    switch (card.getInstruction()) {
        case ChanceCard::GoToNearestStation: {
            std::vector<int> stationPositions;
            int boardSize = 0;

            for (int i = 0;; ++i) {
                try {
                    Tile& tile = board_.getCurrentTile(i);
                    ++boardSize;

                    if (dynamic_cast<RailRoadTile*>(&tile) != nullptr) {
                        stationPositions.push_back(i);
                    }
                } catch (...) {
                    break;
                }
            }

            if (stationPositions.empty() || boardSize == 0) {
                view_.showMessage("[CHANCE] Stasiun tidak ditemukan di papan.\n");
                return;
            }

            const int currentPos = player.getPosition();
            int bestPos = stationPositions.front();
            int bestForwardDistance = boardSize;

            for (int stationPos : stationPositions) {
                int distance = (stationPos - currentPos + boardSize) % boardSize;
                if (distance == 0) {
                    distance = boardSize;
                }

                if (distance < bestForwardDistance) {
                    bestForwardDistance = distance;
                    bestPos = stationPos;
                }
            }

            player.setPosition(bestPos);
            view_.showMessage("[CHANCE] Kamu dipindahkan ke stasiun terdekat.\n");
            break;
        }

        case ChanceCard::MoveBackThreeTiles: {
            int boardSize = 0;
            for (int i = 0;; ++i) {
                try {
                    (void)board_.getCurrentTile(i);
                    ++boardSize;
                } catch (...) {
                    break;
                }
            }

            if (boardSize == 0) {
                view_.showMessage("[CHANCE] Papan tidak valid.\n");
                return;
            }

            const int newPos = (player.getPosition() - 3 + boardSize) % boardSize;
            player.setPosition(newPos);
            view_.showMessage("[CHANCE] Kamu mundur 3 petak.\n");
            break;
        }

        case ChanceCard::GoToJail:
            player.setPosition(board_.getJailPosition());
            player.setStatus(Player::JAILED);
            player.resetJailTurn();
            view_.showMessage("[CHANCE] Kamu masuk penjara.\n");
            break;

        default:
            break;
    }
}

void PropertyCoordinator::processRedeem(Player& player) {
    std::vector<PropertyTile*> mortgagedProperties = player.getMortgagedProperties();
    if (mortgagedProperties.empty()) {
        view_.showMessage("KOSONG GA ADA WOII!\n");
        return;
    }

    view_.showMessage("Berikut ini properti yang sedang digadaikan:\n");
    for (size_t i = 0; i < mortgagedProperties.size(); ++i) {
        PropertyTile* t = mortgagedProperties.at(i);
        view_.showMessage(std::to_string(i + 1) + ". " + t->getLetterCode() + " - " + t->getTileName() + " | Harga beli: M" + std::to_string(t->getPurchasePrice()) + " | Harga gadai: M" + std::to_string(t->getSellingPrice()) + "\n");
    }

    view_.showMessage("Pilih nomor properti untuk ditebus (0 untuk batal): ");
    int toBeRedeemed = command_.getInt(0, mortgagedProperties.size());

    if (toBeRedeemed == 0) {
        view_.showMessage("Batal tebus.\n");
        return;
    }

    PropertyTile* chosen = mortgagedProperties.at(toBeRedeemed - 1);
    int price = chosen->getPurchasePrice();
    if (player.getBalance() < price) {
        view_.showMessage("Saldo tidak cukup untuk menebus properti ini.\n");
        return;
    }

    player.deductMoney(price);
    chosen->setPropertyStatus(PropertyStatus::OWNED);

    view_.showMessage("Properti berhasil ditebus. Sisa saldo: M" + std::to_string(player.getBalance()) + "\n");
}

void PropertyCoordinator::processPayRent(Player& player, Tile& currentTile) {
    PropertyTile* propertyTile = dynamic_cast<PropertyTile*>(&currentTile);
    if (!propertyTile) {
        return;
    }

    for (size_t i = 0; i < players_.size(); i++) {
        Player* owner = players_[i].get();
        if (owner && owner->getUsername() == propertyTile->getOwnerUsername()) {
            int rent = 0;
            if (StreetTile* streetTile = dynamic_cast<StreetTile*>(propertyTile)) {
                bool cg = board_.isCompletedColourGroup(owner->getUsername(), propertyTile->getColourBlock());
                rent = streetTile->calculateRentPrice(cg);
            }
            else if (RailRoadTile* railRoadTile = dynamic_cast<RailRoadTile*>(propertyTile)) {
                int count = board_.countOwnedRailRoadTile(owner->getUsername());
                rent = railRoadTile->calculateRentPrice(count);
            }
            else if (UtilityTile* utilityTile = dynamic_cast<UtilityTile*>(propertyTile)) {
                int count = board_.countOwnedUtilityTile(owner->getUsername());
                int dice = dice_.getRollResult();
                rent = utilityTile->calculateRentPrice(count, dice);
            }
            if (player.getBalance() < rent) {
                processBankruptcyFlow(player, *owner, rent);
                return;
            }
            owner->addMoney(rent);
            player.deductMoney(rent);

            view_.showMessage("Kamu telah membayar sewa sebesar M" + std::to_string(rent) + " kepada " + owner->getUsername() + "\n");
            return;
        }
    }
}

void PropertyCoordinator::processBuyBuilding(Player& player) {
    view_.showMessage("ini ni yg memenuhi syarat");

    std::map<std::string, std::vector<PropertyTile*>> completeColourGroup = player.getCompleteColourGroups(board_.getCountTilesForEachColourBlock());
    if (completeColourGroup.empty()) {
        view_.showMessage("Tidak ada color group yang memenuhi syarat untuk dibangun\n");
        return;
    }
    view_.showMessage("Nah ini dia bla bla bla bla bla");

    int selected = command_.getInt(0, completeColourGroup.size());

    if (selected == 0) {
        return;
    }

    auto it = std::next(completeColourGroup.begin(), selected - 1);
    std::vector<PropertyTile*> validProp = it->second;

    selected = command_.getInt(0, completeColourGroup.size());
    if (selected == 0) {
        return;
    }

    StreetTile* chosenPropTile = dynamic_cast<StreetTile*>(validProp.at(selected - 1));

    if (chosenPropTile != nullptr) {
        int price = chosenPropTile->getBuildNextBuildingPrice();
        if (player.getBalance() < price) {
            view_.showMessage("Anda tidak berhasil bangun, silahkan tidur lagi!\n");
            return;
        }

        try {
            chosenPropTile->upgradeBuilding();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            view_.showMessage("Duit lu ga cukup!\n");
            return;
        }

        player.deductMoney(price);
        view_.showMessage("Lalalalallal aku berhasil upgrade!\n");
    }
}

void PropertyCoordinator::processBankruptcyFlow(Player& payer, Player& owner, int rent) {
    LiquidationManager liquidation(view_, command_);

    view_.showMessage("Uang tidak cukup. Memulai proses likuidasi...\n");

    bool success = liquidation.runLiquidation(payer, rent);

    if (success) {
        view_.showMessage("Likuidasi berhasil. Membayar kewajiban...\n");

        payer.deductMoney(rent);
        owner.addMoney(rent);

        view_.showMessage(payer.getUsername() + " membayar M" + std::to_string(rent) + " kepada " + owner.getUsername() + "\n");
    } else {
        view_.showMessage("Likuidasi gagal. Pemain bangkrut.\n");

        processBankruptcyToPlayer(payer, owner);
    }
}

void PropertyCoordinator::processBankruptcyToPlayer(Player& payer, Player& owner) {
    // transfer uang
    int money = payer.getBalance();
    if (money > 0) {
        payer.deductMoney(money);
        owner.addMoney(money);
    }

    // transfer property
    std::vector<PropertyTile*> properties = payer.getProperties();

    for (size_t i = 0; i < properties.size(); i++) {
        PropertyTile* property = properties[i];
        if (property == nullptr) continue;

        payer.removeProperty(property);
        owner.addProperty(property);

        property->setOwnerUsername(owner.getUsername());
    }

    payer.setStatus(Player::PlayerStatus::BANKRUPT);

    view_.showMessage(payer.getUsername() + " bangkrut kepada " + owner.getUsername() + "\n");
}

void PropertyCoordinator::processMortgage(Player& player) {
    view_.showMessage("Selamat datang di proses pegadain!\n");

    std::map<std::string, std::vector<PropertyTile*>> ownedProperty = player.getOwnedPropertiesGroupByColourGroups();

    if (ownedProperty.empty()) {
        view_.showMessage("Ga ada property yang bisa digadai");
        return;
    }

    std::vector<PropertyTile*> linearOrdered;

    view_.showMessage("Daftar properti yang dapat digadaikan:\n");
    int count = 0;

    for (auto& keyValue : ownedProperty) {
        std::vector<PropertyTile*>& vec = keyValue.second;
        for (auto& val : vec) {
            linearOrdered.push_back(val);
            ++count;
            view_.showMessage("" + std::to_string(count) + ". " + val->getLetterCode() + " - " + val->getTileName() + " | Harga gadai: M" + std::to_string(val->getSellingPrice()) + "\n");
        }
    }

    if (count == 0) {
        view_.showMessage("Tidak ada properti yang dapat digadaikan.\n");
        return;
    }

    view_.showMessage("Pilih nomor properti untuk digadaikan (0 untuk batal): ");
    int selected = command_.getInt(0, count);
    if (selected == 0) {
        view_.showMessage("Batal gadai.\n");
        return;
    }

    PropertyTile& selectedTile = *linearOrdered.at(selected - 1);
    std::vector<PropertyTile*> members = ownedProperty[selectedTile.getColourBlock()];

    bool foundHasBuilding = false;
    for (size_t i = 0; i < count; i++) {
        if (members.at(i)->hasBuilding()) {
            foundHasBuilding = true;
            break;
        }
    }

    if (!foundHasBuilding) {
        selectedTile.setToMortgaged();
        player.addMoney(selectedTile.getSellingPrice());
        view_.showMessage("hore kamu dapat duit bla bla bla\n");
    } else {
        view_.showMessage("di tile ini masih ada bangunannya, tidak dapat digadaikan! jual dulu gih");
        view_.showMessage("Ini dia ni yg masih ada\n");

        bool wantToSellAllBuildings = command_.askWantToSellAllBuildings("mau jual semua gaa\n");
        if (wantToSellAllBuildings) {
            int earnedSum = 0;
            for (size_t i = 0; i < members.size(); i++) {
                earnedSum += members.at(i)->sellAllBuildings();
                view_.showMessage("Ceklik..\n");
            }
            view_.showMessage("total:///");
            player.addMoney(earnedSum);
        } else {
            view_.showMessage("baiklah bye");
            return;
        }

        if (command_.getBool("Mau lanjut gadai ga\n")) {
            selectedTile.setToMortgaged();
            player.addMoney(selectedTile.getSellingPrice());
            view_.showMessage("hore kamu dapat duit bla bla bla\n");
            return;
        }
    }
}
