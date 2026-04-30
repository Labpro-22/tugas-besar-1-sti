#include "controllers/PropertyCoordinator.hpp"
#include "models/exception/SessionException/BankruptcyException.hpp"
#include "utils/Formatter.hpp"

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
    view_.showMessage("Daftar properti milikmu:\n");

    std::vector<PropertyTile*> owned = player.getProperties(); 
    if (owned.empty()) {
        view_.showMessage("Kamu belum memiliki properti apa pun untuk mengadakan festival!\n");
        return;
    }

    for (PropertyTile* pt : owned) {
        view_.showMessage("- " + pt->getLetterCode() + " (" + pt->getTileName() + ")\n");
    }

    PropertyTile* propTile = nullptr;
    std::string chosenPropCode;

    while (true) {
        view_.showMessage("\nMasukkan kode properti: ");
        
        chosenPropCode = command_.getString(); 

        if (!board_.has(chosenPropCode)) {
            view_.showMessage("-> Kode properti tidak valid!\n");
            continue; 
        }

        if (!player.hasProperty(chosenPropCode)) {
            view_.showMessage("-> Properti bukan milikmu!\n");
            continue; 
        }

        propTile = &player.getProperty(chosenPropCode);
        break; 
    }

    auto getPreviewRent = [&](PropertyTile* pt) -> int {
        StreetTile* st = dynamic_cast<StreetTile*>(pt);
        if (st) {
            std::map<std::string, std::vector<PropertyTile*>> completeGroups = player.getCompleteColourGroups(board_.getCountTilesForEachColourBlock());
            bool isMonopoly = (completeGroups.find(st->getColourBlock()) != completeGroups.end());
            return st->calculateRentPrice(isMonopoly);
        }

        RailRoadTile* rt = dynamic_cast<RailRoadTile*>(pt);
        if (rt) {
            int rrCount = 0;
            for (PropertyTile* prop : owned) {
                if (dynamic_cast<RailRoadTile*>(prop)) rrCount++;
            }
            return rt->calculateRentPrice(rrCount);
        }

        UtilityTile* ut = dynamic_cast<UtilityTile*>(pt);
        if (ut) {
            int utCount = 0;
            for (PropertyTile* prop : owned) {
                if (dynamic_cast<UtilityTile*>(prop)) utCount++;
            }
            return ut->calculateRentPrice(utCount, 1); 
        }
        return 0;
    };

    int rentAwal = getPreviewRent(propTile); 

    bool isUtility = dynamic_cast<UtilityTile*>(propTile) != nullptr;
    std::string utilityNote = isUtility ? " x Dadu" : "";

    if (!propTile->festivalActive()) {
        propTile->setFestivalMultiplier(2);
        propTile->setFestivalDuration(3);
        
        int rentSekarang = getPreviewRent(propTile);

        view_.showMessage("\nEfek festival aktif!\n\n");
        view_.showMessage("Sewa awal     : " + Formatter::formattingMoney(rentAwal) + utilityNote + "\n");
        view_.showMessage("Sewa sekarang : " + Formatter::formattingMoney(rentSekarang) + utilityNote + "\n");
        view_.showMessage("Durasi        : 3 giliran\n");

    } else if (propTile->alreadyMaxMultiplier()) {
        propTile->setFestivalDuration(3);

        view_.showMessage("\nEfek sudah maksimum (harga sewa sudah digandakan tiga kali)\n\n");
        view_.showMessage("Durasi di-reset menjadi: 3 giliran\n");

    } else {
        propTile->doubleTheMultiplier();
        propTile->setFestivalDuration(3);
        
        int rentSekarang = getPreviewRent(propTile);

        view_.showMessage("\nEfek diperkuat!\n\n");
        view_.showMessage("Sewa sebelumnya : " + Formatter::formattingMoney(rentAwal) + utilityNote + "\n");
        view_.showMessage("Sewa sekarang   : " + Formatter::formattingMoney(rentSekarang) + utilityNote + "\n");
        view_.showMessage("Durasi di-reset menjadi: 3 giliran\n");
    }
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
            player.setJailTurn(3);
            view_.showMessage("[CHANCE] Kamu masuk penjara.\n");
            break;

        default:
            break;
    }
}

void PropertyCoordinator::processTakeCommunityChest(Player& player) {
    CommunityChestCard card = CommunityChestCard::randomCard();
    view_.showMessage("[COMMUNITY CHEST] " + card.getDescription() + "\n");

    switch (card.getInstruction()) {
        case CommunityChestCard::BirthdayCollect100FromEachPlayer:
            for (auto& otherPtr : players_) {
                Player* other = otherPtr.get();
                if (other == nullptr || other == &player || other->isBankrupt()) {
                    continue;
                }

                other->deductMoney(100);
                player.addMoney(100);
            }
            break;

        case CommunityChestCard::DoctorFeePay700:
            try {
                player.deductMoney(700);
            }
            catch(const InsufficientFundsException& e) {
                throw CardPaymentFailedException(101, "Gagal memakai Kartu DoctorFeePay700 karena saldo tidak mencukupi\n");
            }
        
            break;

        case CommunityChestCard::ElectionPay200ToEachPlayer : {
                int sumMoney = 0;
                for (auto& otherPtr : players_) {
                    Player* other = otherPtr.get();
                    if (other == nullptr || other == &player || other->isBankrupt()) {
                        continue;
                    }
                    sumMoney += 200;
                }

                if (player.getBalance() < sumMoney) {
                    throw CardPaymentFailedException(101, "Gagal memakai kartu ElectionPay200ToEachPlayer\n");
                }

                // harusnya udh aman di sini
                for (auto& otherPtr : players_) {
                    Player* other = otherPtr.get();
                    if (other == nullptr || other == &player || other->isBankrupt()) {
                        continue;
                    }
                    player.deductMoney(200);
                    other->addMoney(200);
                }
            }
            break;
        
        default:
            break;
    }
}

void PropertyCoordinator::processRedeem(Player& player) {
    std::vector<PropertyTile*> mortgagedProperties = player.getMortgagedProperties();
    
    if (mortgagedProperties.empty()) {
        view_.showMessage("Tidak ada properti yang sedang digadaikan.\n");
        return;
    }

    view_.showMessage("=== Properti yang Sedang Digadaikan ===\n");
    
    auto padR = [](std::string s, size_t w) {
        if (s.length() >= w) return s;
        return s + std::string(w - s.length(), ' ');
    };

    for (size_t i = 0; i < mortgagedProperties.size(); ++i) {
        PropertyTile* t = mortgagedProperties.at(i);
        std::string num = std::to_string(i + 1) + ". ";
        std::string nameInfo = t->getTileName() + " (" + t->getLetterCode() + ")";

        std::string cat = t->getColourBlock();
        if (dynamic_cast<RailRoadTile*>(t)) cat = "STASIUN";
        else if (dynamic_cast<UtilityTile*>(t)) cat = "UTILITAS";
        std::string colorStr = "[" + cat + "]";
        int hargaTebus = t->getPurchasePrice(); 

        view_.showMessage(padR(num + nameInfo, 26) + padR(colorStr, 12) + " [M]  Harga Tebus: " + Formatter::formattingMoney(hargaTebus) + "\n");
    }

    view_.showMessage("\nUang kamu saat ini: " + Formatter::formattingMoney(player.getBalance()) + "\n");
    view_.showMessage("Pilih nomor properti (0 untuk batal): ");
    int toBeRedeemed = command_.getInt(0, mortgagedProperties.size());
    if (toBeRedeemed == 0) {
        return;
    }

    PropertyTile* chosen = mortgagedProperties.at(toBeRedeemed - 1);
    int price = chosen->getPurchasePrice();

    if (player.getBalance() < price) {
        view_.showMessage("\nUang kamu tidak cukup untuk menebus " + chosen->getTileName() + ".\n");
        view_.showMessage("Harga tebus: " + Formatter::formattingMoney(price) + " | Uang kamu: " + Formatter::formattingMoney(player.getBalance()) + "\n");
        return;
    }

    player.deductMoney(price);
    chosen->setPropertyStatus(PropertyStatus::OWNED);

    view_.showMessage("\n" + chosen->getTileName() + " berhasil ditebus!\n");
    view_.showMessage("Kamu membayar " + Formatter::formattingMoney(price) + " ke Bank.\n");
    view_.showMessage("Uang kamu sekarang: " + Formatter::formattingMoney(player.getBalance()) + "\n");
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
            std::string conditionStr = "";

            if (StreetTile* streetTile = dynamic_cast<StreetTile*>(propertyTile)) {
                bool cg = board_.isCompletedColourGroup(owner->getUsername(), propertyTile->getColourBlock());
                rent = streetTile->calculateRentPrice(cg);
                
                int lvl = streetTile->getLevel();
                if (lvl == 0) conditionStr = cg ? "Tanah Kosong (Full Set)" : "Tanah Kosong";
                else if (lvl == 5) conditionStr = "Hotel";
                else conditionStr = std::to_string(lvl) + " rumah";
            }
            else if (RailRoadTile* railRoadTile = dynamic_cast<RailRoadTile*>(propertyTile)) {
                int count = owner->countRailroad(); // Count railroad punya owner
                rent = railRoadTile->calculateRentPrice(count);
                conditionStr = std::to_string(count) + " stasiun dimiliki";
            }
            else if (UtilityTile* utilityTile = dynamic_cast<UtilityTile*>(propertyTile)) {
                int count = owner->countUtilities(); // count utilities punya owner
                int dice = dice_.getRollResult();
                rent = utilityTile->calculateRentPrice(count, dice);
                conditionStr = std::to_string(count) + " utilitas, dadu " + std::to_string(dice);
            }

            int fMult = propertyTile->getFestivalMultiplier();
            if (fMult > 1) {
                conditionStr += " (Festival " + std::to_string(fMult) + "x)";
            }

            view_.showMessage("\nKondisi      : " + conditionStr + "\n");
            view_.showMessage("Sewa         : " + Formatter::formattingMoney(rent) + "\n\n");
            
            if (player.getBalance() < rent) {
                view_.showMessage("Kamu tidak mampu membayar sewa penuh! (" + Formatter::formattingMoney(rent) + ")\n");
                view_.showMessage("Uang kamu saat ini: " + Formatter::formattingMoney(player.getBalance()) + "\n");
                
                processBankruptcyFlow(player, *owner, rent);
                return;
            }

            int oldPlayerBalance = player.getBalance();
            int oldOwnerBalance = owner->getBalance();

            owner->addMoney(rent);
            player.deductMoney(rent);

            auto padR = [](std::string s, size_t w) {
                if (s.length() >= w) return s;
                return s + std::string(w - s.length(), ' ');
            };

            view_.showMessage("Uang kamu     : " + padR(Formatter::formattingMoney(oldPlayerBalance), 8) + " -> " + Formatter::formattingMoney(player.getBalance()) + "\n");
            view_.showMessage("Uang " + padR(owner->getUsername(), 9) + " : " + padR(Formatter::formattingMoney(oldOwnerBalance), 8) + " -> " + Formatter::formattingMoney(owner->getBalance()) + "\n");
            
            return;
        }
    }
}

void PropertyCoordinator::processBuyBuilding(Player& player) {
    std::map<std::string, std::vector<PropertyTile*>> completeColourGroup = player.getCompleteColourGroups(board_.getCountTilesForEachColourBlock());

    if (completeColourGroup.empty()) {
        view_.showMessage("Tidak ada color group yang memenuhi syarat untuk dibangun.\n");
        view_.showMessage("Kamu harus memiliki seluruh petak dalam satu color group terlebih dahulu.\n");
        return;
    }

    view_.showMessage("=== Color Group yang Memenuhi Syarat ===\n");
    int groupIdx = 1;

    auto padR = [](std::string s, size_t w) {
        if (s.length() >= w) return s;
        return s + std::string(w - s.length(), ' ');
    };

    for (auto const& pair : completeColourGroup) {
        std::string color = pair.first;
        view_.showMessage(std::to_string(groupIdx++) + ". [" + color + "]\n");

        for (PropertyTile* pt : pair.second) {
            StreetTile* st = dynamic_cast<StreetTile*>(pt);
            if (st) {
                int lvl = st->getLevel();
                std::string lvlStr = (lvl == 5) ? "Hotel" : std::to_string(lvl) + " rumah";

                std::map<int, int> bp = st->getBuildPrice();
                int bCost = bp.count(1) ? bp.at(1) : (bp.empty() ? 0 : bp.begin()->second);

                view_.showMessage("   - " + padR(st->getTileName() + " (" + st->getLetterCode() + ")", 25) + ": " + padR(lvlStr, 7) + " (Harga rumah: " + Formatter::formattingMoney(bCost) + ")\n");
            }
        }
    }

    view_.showMessage("\nUang kamu saat ini : " + Formatter::formattingMoney(player.getBalance()) + "\n");
    view_.showMessage("Pilih nomor color group (0 untuk batal): ");
    int selectedGrpIdx = command_.getInt(0, completeColourGroup.size());

    if (selectedGrpIdx == 0) return;

    auto it = std::next(completeColourGroup.begin(), selectedGrpIdx - 1);
    std::string selColor = it->first;
    std::vector<PropertyTile*> tilesInGroup = it->second;

    for (PropertyTile* t : tilesInGroup) {
        if (t->getPropertyStatus() == PropertyStatus::MORTGAGED) {
            view_.showMessage("\nGrup [" + selColor + "] memiliki petak yang sedang digadaikan.\n");
            view_.showMessage("Tebus gadai terlebih dahulu sebelum membangun.\n");
            return;
        }
    }

    std::vector<StreetTile*> selStreets;
    for (PropertyTile* t : tilesInGroup) {
        StreetTile* st = dynamic_cast<StreetTile*>(t);
        if (st) selStreets.push_back(st);
    }

    int minLevel = 5;
    for (StreetTile* st : selStreets) {
        if (st->getLevel() < minLevel) minLevel = st->getLevel();
    }

    view_.showMessage("\nColor group [" + selColor + "]:\n");
    bool canBuildAny = false;

    for (size_t i = 0; i < selStreets.size(); i++) {
        StreetTile* st = selStreets[i];
        int lvl = st->getLevel();
        std::string lvlStr = (lvl == 5) ? "Hotel" : std::to_string(lvl) + " rumah";
        std::string statusMark = "";

        if (lvl == 5) {
            statusMark = "<- sudah maksimal";
        } else if (lvl == minLevel) {
            if (lvl == 4) statusMark = "<- siap upgrade ke hotel";
            else statusMark = "<- dapat dibangun";
            canBuildAny = true;
        } else {
            statusMark = "<- bangun petak lain dulu (aturan pemerataan)";
        }

        view_.showMessage(std::to_string(i + 1) + ". " + padR(st->getTileName() + " (" + st->getLetterCode() + ")", 25) + ": " + padR(lvlStr, 7) + " " + statusMark + "\n");
    }

    if (!canBuildAny) {
        view_.showMessage("\nSemua properti di color group ini sudah mencapai level maksimal (Hotel).\n");
        return;
    }

    if (minLevel == 4) {
        view_.showMessage("\nSeluruh color group [" + selColor + "] sudah memiliki 4 rumah. Siap di-upgrade ke hotel!\n");
    }

    view_.showMessage("\nPilih petak (0 untuk batal): ");
    int selectedPropIdx = command_.getInt(0, selStreets.size());
    
    if (selectedPropIdx == 0) return;

    StreetTile* chosenPropTile = selStreets[selectedPropIdx - 1];

    if (chosenPropTile->getLevel() != minLevel || chosenPropTile->getLevel() == 5) {
        view_.showMessage("Petak ini belum dapat dibangun saat ini (aturan pemerataan atau sudah maksimal).\n");
        return;
    }

    std::map<int, int> bp = chosenPropTile->getBuildPrice();
    int price = bp.count(1) ? bp.at(1) : (bp.empty() ? 0 : bp.begin()->second);

    if (player.getBalance() < price) {
        view_.showMessage("\nUang kamu tidak cukup untuk membangun di petak ini!\n");
        view_.showMessage("Harga: " + Formatter::formattingMoney(price) + " | Uang kamu: " + Formatter::formattingMoney(player.getBalance()) + "\n");
        return;
    }

    if (chosenPropTile->getLevel() == 4) {
        if (!command_.getBool("\nUpgrade ke hotel? Biaya: " + Formatter::formattingMoney(price))) {
            view_.showMessage("Batal upgrade.\n");
            return;
        }
    }

    try {
        chosenPropTile->upgradeBuilding();
    } catch (const std::exception& e) {
        view_.showMessage(std::string(e.what()) + "\n");
        return;
    }

    player.deductMoney(price);

    int newLvl = chosenPropTile->getLevel();
    view_.showMessage("\n");
    if (newLvl == 5) {
        view_.showMessage(chosenPropTile->getTileName() + " di-upgrade ke Hotel!\n");
    } else {
        view_.showMessage("Kamu membangun 1 rumah di " + chosenPropTile->getTileName() + ". Biaya: " + Formatter::formattingMoney(price) + "\n");
    }

    view_.showMessage("Uang tersisa: " + Formatter::formattingMoney(player.getBalance()) + "\n");
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
        try
        {
            processBankruptcyToPlayer(payer, owner);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
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
    // throw BankruptcyException(payer.getUsername());
}

void PropertyCoordinator::processMortgage(Player& player) {
    std::map<std::string, std::vector<PropertyTile*>> ownedProperty = player.getOwnedPropertiesGroupByColourGroups();

    std::vector<PropertyTile*> linearOrdered;
    for (auto& keyValue : ownedProperty) {
        for (auto& val : keyValue.second) {
            if (val->getPropertyStatus() != PropertyStatus::MORTGAGED) {
                linearOrdered.push_back(val);
            }
        }
    }

    if (linearOrdered.empty()) {
        view_.showMessage("Tidak ada properti yang dapat digadaikan saat ini.\n");
        return;
    }

    view_.showMessage("=== Properti yang Dapat Digadaikan ===\n");
    size_t count = 0;

    auto padR = [](std::string s, size_t w) {
        if (s.length() >= w) return s;
        return s + std::string(w - s.length(), ' ');
    };

    for (auto& val : linearOrdered) {
        ++count;
        std::string num = std::to_string(count) + ". ";
        std::string nameInfo = val->getTileName() + " (" + val->getLetterCode() + ")";
        std::string cat = val->getColourBlock();
        if (dynamic_cast<RailRoadTile*>(val)) cat = "STASIUN";
        else if (dynamic_cast<UtilityTile*>(val)) cat = "UTILITAS";
        std::string colorStr = "[" + cat + "]";
        view_.showMessage(padR(num + nameInfo, 30) + padR(colorStr, 12) + " Nilai Gadai: " + Formatter::formattingMoney(val->getMortgageValue()) + "\n");
    }

    view_.showMessage("\nPilih nomor properti (0 untuk batal): ");
    int selected = command_.getInt(0, count);
    if (selected == 0) {
        return;
    }

    PropertyTile* selectedTile = linearOrdered.at(selected - 1);
    std::string targetColor = selectedTile->getColourBlock();
    std::vector<PropertyTile*> members = ownedProperty[targetColor];
    bool foundHasBuilding = false;
    std::vector<StreetTile*> streetsWithBuildings;

    for (PropertyTile* member : members) {
        StreetTile* street = dynamic_cast<StreetTile*>(member);
        if (street && street->getLevel() > 0) {
            foundHasBuilding = true;
            streetsWithBuildings.push_back(street);
        }
    }

    if (!foundHasBuilding) {
        selectedTile->setToMortgaged();
        player.addMoney(selectedTile->getMortgageValue());
        
        view_.showMessage(selectedTile->getTileName() + " berhasil digadaikan.\n");
        view_.showMessage("Kamu menerima " + Formatter::formattingMoney(selectedTile->getMortgageValue()) + " dari Bank.\n");
        view_.showMessage("Uang kamu sekarang: " + Formatter::formattingMoney(player.getBalance()) + "\n");
        view_.showMessage("Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.\n");
    } else {
        view_.showMessage("\n" + selectedTile->getTileName() + " tidak dapat digadaikan!\n");
        view_.showMessage("Masih terdapat bangunan di color group [" + targetColor + "].\n");
        view_.showMessage("Bangunan harus dijual terlebih dahulu.\n\n");

        view_.showMessage("Daftar bangunan di color group [" + targetColor + "]:\n");
        int idx = 1;
        for (StreetTile* street : streetsWithBuildings) {
            int level = street->getLevel();
            std::string lvlStr = (level == 5) ? "Hotel" : std::to_string(level) + " rumah";
            std::map<int, int> buildMap = street->getBuildPrice();
            int costPerBuilding = buildMap.count(1) ? buildMap.at(1) : (buildMap.empty() ? 0 : buildMap.begin()->second);
            int sellValue = (level == 5 ? 5 : level) * (costPerBuilding / 2);
            
            view_.showMessage(std::to_string(idx) + ". " + padR(street->getTileName() + " (" + street->getLetterCode() + ")", 25) + "- " + padR(lvlStr, 8) + " -> Nilai jual bangunan: " + Formatter::formattingMoney(sellValue) + "\n");
            idx++;
        }

        bool wantToSell = command_.getBool("\nJual semua bangunan color group [" + targetColor + "]?"); 
        
        if (wantToSell) {
            int earnedSumTotal = 0;
            for (StreetTile* street : streetsWithBuildings) {
                int earned = street->sellAllBuildings();
                earnedSumTotal += earned;
                view_.showMessage("Bangunan " + street->getTileName() + " terjual. Kamu menerima " + Formatter::formattingMoney(earned) + ".\n");
            }
            player.addMoney(earnedSumTotal);
            view_.showMessage("Uang kamu sekarang: " + Formatter::formattingMoney(player.getBalance()) + "\n");
        } else {
            return;
        }

        view_.showMessage("\nLanjut menggadaikan " + selectedTile->getTileName() + "? (y/n): ");
        if (command_.getBool("")) {
            selectedTile->setToMortgaged();
            player.addMoney(selectedTile->getMortgageValue());
            
            view_.showMessage(selectedTile->getTileName() + " berhasil digadaikan.\n");
            view_.showMessage("Kamu menerima " + Formatter::formattingMoney(selectedTile->getMortgageValue()) + " dari Bank.\n");
            view_.showMessage("Uang kamu sekarang: " + Formatter::formattingMoney(player.getBalance()) + "\n");
            view_.showMessage("Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.\n");
        }
    }
}