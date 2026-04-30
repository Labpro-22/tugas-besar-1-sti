// Writer.cpp
#include "models/io/Writer.hpp"

#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/tile/property_tile/StreetTile.hpp"
#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/tile/property_tile/UtilityTile.hpp"
#include "models/card/skillcard/MoveCard.hpp"
#include "models/card/skillcard/DiscountCard.hpp"
#include "models/card/skillcard/ShieldCard.hpp"
#include "models/card/skillcard/TeleportCard.hpp"
#include "models/card/skillcard/LassoCard.hpp"
#include "models/card/skillcard/DemolitionCard.hpp"
#include "models/exception/SessionException/SessionException.hpp"

#include <filesystem>
#include <stdexcept>

Writer::Writer(std::string folderName) : folderName_(folderName) {
    std::filesystem::create_directories(folderName_);
}

Writer::~Writer() = default;

std::string Writer::buildPath(const std::string& filename) const {
    return folderName_ + "/" + filename;
}

std::string Writer::playerStatusToString(const Player& player) const {
    if (player.getStatus() == Player::PlayerStatus::BANKRUPT) return "BANKRUPT";
    if (player.getStatus() == Player::PlayerStatus::JAILED) return "JAILED_" + std::to_string(player.getCountJail());
    return "ACTIVE";
}

std::string Writer::propertyStatusToString(const PropertyTile& property) const {
    if (property.getPropertyStatus() == MORTGAGED) return "MORTGAGED";
    if (property.getPropertyStatus() == OWNED) return "OWNED";
    return "BANK";
}

std::string Writer::propertyTypeToString(const PropertyTile& property) const {
    if (dynamic_cast<const StreetTile*>(&property) != nullptr) return "street";
    if (dynamic_cast<const RailRoadTile*>(&property) != nullptr) return "railroad";
    if (dynamic_cast<const UtilityTile*>(&property) != nullptr) return "utility";
    return "property";
}

std::string Writer::buildingLevelToSaveString(const PropertyTile& property) const {
    const StreetTile* street = dynamic_cast<const StreetTile*>(&property);
    if (street == nullptr) return "0";

    int level = street->getLevel();
    if (level >= 5) return "H";
    return std::to_string(level);
}

void Writer::writeHeader(std::ofstream& file, int currentTurn, int maxTurn, int playerCount) {
    file << currentTurn << " " << maxTurn << "\n";
    file << playerCount << "\n";
}

void Writer::writeSinglePlayerState(std::ofstream& file, const Player& player, const Board& board) {
    Tile& currentTile = const_cast<Board&>(board).getCurrentTile(player.getPosition());

    file << player.getUsername() << " "
         << player.getBalance() << " "
         << currentTile.getLetterCode() << " "
         << playerStatusToString(player) << "\n";

    std::vector<SkillCard*> cards = const_cast<Player&>(player).getSkillCards();
    file << cards.size() << "\n";

    for (SkillCard* card : cards) {
        if (card != nullptr) {
            writeCardState(file, *card);
        }
    }
}

void Writer::writeCardState(std::ofstream& file, const SkillCard& card) {
    file << card.getName();

    const MoveCard* moveCard = dynamic_cast<const MoveCard*>(&card);
    if (moveCard != nullptr) {
        file << " " << moveCard->getSteps();
    }

    const DiscountCard* discountCard = dynamic_cast<const DiscountCard*>(&card);
    if (discountCard != nullptr) {
        file << " " << discountCard->getDiscountPercent()
             << " " << discountCard->getRemainingTurns();
    }

    file << "\n";
}

void Writer::writePlayerStates(
    std::ofstream& file,
    const std::vector<std::unique_ptr<Player>>& players,
    const Board& board
) {
    for (const auto& player : players) {
        writeSinglePlayerState(file, *player, board);
    }
}

void Writer::writeTurnOrder(std::ofstream& file, const std::vector<std::unique_ptr<Player>>& players) {
    for (std::size_t i = 0; i < players.size(); ++i) {
        if (i > 0) file << " ";
        file << players[i]->getUsername();
    }
    file << "\n";
}

void Writer::writeCurrentPlayer(
    std::ofstream& file,
    const std::vector<std::unique_ptr<Player>>& players,
    int currentPlayerIndex
) {
    if (players.empty()) {
        throw std::runtime_error("Tidak ada pemain untuk disimpan.");
    }

    int index = currentPlayerIndex % static_cast<int>(players.size());
    if (index < 0) index += players.size();

    file << players[index]->getUsername() << "\n";
}

void Writer::writePropertyStates(std::ofstream& file, const Board& board) {
    std::vector<PropertyTile*> properties;

    for (int i = 0; i < const_cast<Board&>(board).getSize(); ++i) {
        Tile& tile = const_cast<Board&>(board).getCurrentTile(i);
        PropertyTile* property = dynamic_cast<PropertyTile*>(&tile);

        if (property != nullptr) {
            properties.push_back(property);
        }
    }

    file << properties.size() << "\n";

    for (PropertyTile* property : properties) {
        file << property->getLetterCode() << " "
             << propertyTypeToString(*property) << " "
             << property->getOwnerUsername() << " "
             << propertyStatusToString(*property) << " "
             << property->getFestivalMultiplier() << " "
             << property->getFestivalDuration() << " "
             << buildingLevelToSaveString(*property) << "\n";
    }
}

void Writer::writeDeckState(std::ofstream& file, const Deck<SkillCard>& deck) {
    std::vector<std::string> cardNames = deck.getDrawableCardNames();

    file << cardNames.size() << "\n";

    for (const std::string& name : cardNames) {
        file << name << "\n";
    }
}

void Writer::writeTransactionLog(std::ofstream& file, const std::vector<std::string>& logs) {
    file << logs.size() << "\n";

    for (const std::string& log : logs) {
        file << log << "\n";
    }
}

void Writer::saveGameState(
    int currentTurn,
    int maxTurn,
    const std::vector<std::unique_ptr<Player>>& players,
    const Board& board,
    const Deck<SkillCard>& deck,
    const std::vector<std::string>& logs,
    const std::string& filename
) {
    if (players.empty()) {
        throw SessionException(400, "Tidak bisa save: pemain kosong.");
    }

    std::string path = buildPath(filename);
    std::ofstream file(path);

    if (!file.is_open()) {
        throw SessionException(401, "Tidak bisa membuka file save: " + path);
    }

    writeHeader(file, currentTurn, maxTurn, static_cast<int>(players.size()));
    writePlayerStates(file, players, board);
    writeTurnOrder(file, players);
    file << players[0]->getUsername() << "\n";
    writePropertyStates(file, board);
    writeDeckState(file, deck);
    writeTransactionLog(file, logs);

    if (!file.good()) {
        throw SessionException(402, "Gagal menulis save file: " + path);
    }
}