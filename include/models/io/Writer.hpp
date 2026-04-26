// Writer.hpp
#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "models/board/Board.hpp"
#include "models/player/Player.hpp"
#include "models/card/Deck.hpp"
#include "models/card/skillcard/SkillCard.hpp"

class Writer {
private:
    std::string folderName_;
    std::string buildPath(const std::string& filename) const;

    std::string playerStatusToString(const Player& player) const;
    std::string propertyStatusToString(const PropertyTile& property) const;
    std::string propertyTypeToString(const PropertyTile& property) const;
    std::string buildingLevelToSaveString(const PropertyTile& property) const;

    void writeHeader(std::ofstream& file, int currentTurn, int maxTurn, int playerCount);
    void writePlayerStates(std::ofstream& file, const std::vector<std::unique_ptr<Player>>& players, const Board& board);
    void writeSinglePlayerState(std::ofstream& file, const Player& player, const Board& board);
    void writeCardState(std::ofstream& file, const SkillCard& card);
    void writeTurnOrder(std::ofstream& file, const std::vector<std::unique_ptr<Player>>& players);
    void writeCurrentPlayer(std::ofstream& file, const std::vector<std::unique_ptr<Player>>& players, int currentPlayerIndex);
    void writePropertyStates(std::ofstream& file, const Board& board);
    void writeDeckState(std::ofstream& file, const Deck<SkillCard>& deck);
    void writeTransactionLog(std::ofstream& file, const std::vector<std::string>& logs);

public:
    explicit Writer(std::string folderName);
    ~Writer();

    void saveGameState(
        int currentTurn,
        int maxTurn,
        const std::vector<std::unique_ptr<Player>>& players,
        const Board& board,
        const Deck<SkillCard>& deck,
        const std::vector<std::string>& logs,
        const std::string& filename
    );
};