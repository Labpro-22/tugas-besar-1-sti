#include "controllers/GameController.hpp"

#include "models/tile/property_tile/StreetTile.hpp"
#include "models/tile/property_tile/UtilityTile.hpp"

#include <iostream>
#include <iterator>
#include <map>

void GameController::processMovement(Player& p, int firstDisplacement) {
	Tile& nextTile = board_.moveToNextTile(p.move(firstDisplacement));

	p.setPosition(nextTile.getTileID());
	p.setLastDiceTotal(firstDisplacement);
	OnLandResult result = nextTile.onLand(p, command_, view_);

	switch (result) {
		case OnLandResult::TriggerAuction:
			break;
		case OnLandResult::TriggerBankruptcyAuction: {
			auction_.runBankruptcyAuction(p, command_);
			break;
		}
		case OnLandResult::TakeChanceCard:
			processTakeChanceCard(p);
			break;
		case OnLandResult::TakeCommunityChest:
			processTakeCommunityChest(p);
			break;
		case OnLandResult::Festival:
			processFestival(p);
			break;
		case OnLandResult::TriggerMoveToJail:
			p.setPosition(board_.getJailPosition());
			view_.showMessage("Kamu dipindahkan ke penjara");
			break;
		case OnLandResult::TriggerTryToPayRent:
			processPayRent(p, nextTile);
			break;
		case OnLandResult::Done:
			break;
		default:
			break;
	}
}

void GameController::processFestival(Player& p) {
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

		if (!p.hasProperty(chosenPropCode)) {
			view_.showMessage("Tetot\n");
			continue;
		}

		propTile = &p.getProperty(chosenPropCode);

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

void GameController::processTakeChanceCard(Player& p) {
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

			const int currentPos = p.getPosition();
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

			p.setPosition(bestPos);
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

			const int newPos = (p.getPosition() - 3 + boardSize) % boardSize;
			p.setPosition(newPos);
			view_.showMessage("[CHANCE] Kamu mundur 3 petak.\n");
			break;
		}

		case ChanceCard::GoToJail:
			p.setPosition(board_.getJailPosition());
			p.setStatus(Player::JAILED);
			p.resetJailTurn();
			view_.showMessage("[CHANCE] Kamu masuk penjara.\n");
			break;

		default:
			break;
	}
}

void GameController::processRedeem(Player& p) {
	std::vector<PropertyTile *> mortgagedProperties = p.getMortgagedProperties();
	if (mortgagedProperties.empty()) {
		view_.showMessage("KOSONG GA ADA WOII!\n");
		return;
	}

	view_.showMessage("Berikut ini properti yang sedang digadaikan!\n");
	int toBeRedeemed = command_.getInt(0, mortgagedProperties.size());

	if (toBeRedeemed == 0) {
		view_.showMessage("Cancels");
		return;
	}

	if (p.getBalance() < mortgagedProperties.at(toBeRedeemed - 1)->getPurchasePrice()) {
		view_.showMessage("Gagals");
		return;
	}

	p.deductMoney(mortgagedProperties.at(toBeRedeemed - 1)->getPurchasePrice());
	mortgagedProperties.at(toBeRedeemed - 1)->setPropertyStatus(PropertyStatus::OWNED);

	view_.showMessage("Yip yip show message sisa uangnya sama prop yg bs ditebus juga!\n");
}

void GameController::processPayRent(Player& p, Tile& currentTile) {
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
			if (p.getBalance() < rent) {
				processBankruptcyFlow(p, *owner);
				return;
			}
			owner->addMoney(rent);
			p.deductMoney(rent);

			view_.showMessage("Atur-atur mo tampilannya gimana :VVVVVVVVVVVVVVVVVV");
			return;
		}
	}
}

void GameController::processBuyBuilding(Player& p) {
	view_.showMessage("ini ni yg memenuhi syarat");

	std::map<std::string, std::vector<PropertyTile*>> completeColourGroup = p.getCompleteColourGroups(board_.getCountTilesForEachColourBlock());
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
		if (price < p.getBalance()) {
			view_.showMessage("Anda tidak berhasil bangun, silahkan tidur lagi!\n");
			return;
		}

		try {
			chosenPropTile->upgradeBuilding();
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
			view_.showMessage("Duit lu ga cukup!\n");
			return;
		}

		p.deductMoney(price);
		view_.showMessage("Lalalalallal aku berhasil upgrade!\n");
	}
}

void GameController::processBankruptcyFlow(Player& payer, Player& owner) {
	(void)payer;
	(void)owner;
	// Properti berstatus MORTGAGED tidak dapat langsung dijual ke Bank
	// dalam proses likuidasi.
}

void GameController::processMortgage(Player& p) {
	view_.showMessage("Selamat datang di proses pegadain!\n");

	std::map<std::string, std::vector<PropertyTile*>> ownedProperty = p.getOwnedPropertiesGroupByColourGroups();

	if (ownedProperty.empty()) {
		view_.showMessage("Ga ada property yang bisa digadai");
		return;
	}

	std::vector<PropertyTile*> linearOrdered;

	view_.showMessage("Hayo milih dulu\n");
	int count = 0;

	for (auto& keyValue : ownedProperty) {
		std::vector<PropertyTile*>& vec = keyValue.second;
		for (auto& val : vec) {
			linearOrdered.push_back(val);
			count++;
		}
	}

	int selected = command_.getInt(0, count);
	if (selected == 0) {
		view_.showMessage("ga jadi gadai\n");
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
		p.addMoney(selectedTile.getSellingPrice());
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
			p.addMoney(earnedSum);
		} else {
			view_.showMessage("baiklah bye");
			return;
		}

		if (command_.getBool("Mau lanjut gadai ga\n")) {
			selectedTile.setToMortgaged();
			p.addMoney(selectedTile.getSellingPrice());
			view_.showMessage("hore kamu dapat duit bla bla bla\n");
			return;
		}
	}
}
