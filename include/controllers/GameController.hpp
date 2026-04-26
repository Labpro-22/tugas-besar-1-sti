#pragma once
#include "models/board/Board.hpp"
#include "models/player/Player.hpp"
#include "views/CommandInterface.hpp"
#include "views/GameViewInterface.hpp"
#include "models/dice/Dice.hpp"
#include "models/tile/action_tile/special_tile/PrisonTile.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include "controllers/Auction.hpp"
#include "models/card/Deck.hpp"
#include "models/card/skillcard/SkillCard.hpp"
#include "models/card/skillcard/LassoCard.hpp"
#include "models/card/ChanceCard.hpp"
#include "models/card/CommunityChestCard.hpp"
#include "models/card/skillcard/MoveCard.hpp"
#include "models/card/skillcard/TeleportCard.hpp"
#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/io/Writer.hpp"
#include "models/exception/SessionException/SessionException.hpp"

#include <memory>
#include <vector>

class AuctionCoordinator;
class SkillCardCoordinator;
class PropertyCoordinator;

class GameController {
    public:
        GameController(std::vector<std::unique_ptr<Player>> players, 
            Board& board, Dice& dice, GameViewInterface& view, 
            CommandInterface& command, Deck<SkillCard>& specialCardDeck_,
            const std::string& saveFolder);
        ~GameController();

        void setCurrentTurn(int latestTurn);
        void playGame(int latesTurn, int maxTurn);
    private:
        // Deck<SpecialCard>...
        Deck<SkillCard>& specialCardDeck_;
        std::vector<std::string> transactionLog_;
        Board& board_;
        Dice& dice_;
        GameViewInterface& view_;
        CommandInterface& command_;
        std::vector<std::unique_ptr<Player>> players_;
        Auction auction_;
        std::unique_ptr<AuctionCoordinator> auctionCoordinator_;
        std::unique_ptr<SkillCardCoordinator> skillCardCoordinator_;
        std::unique_ptr<PropertyCoordinator> propertyCoordinator_;
        std::unique_ptr<Writer> writer_;

        int currentTurn_;
        int goSalary_;
        int jailFine_;
        
        void processTurn(Player& p);
        void processMovement(Player& p, int firstDisplacement);
        void processRollDice(Player& p);
        bool processRandomDice(Player& p);
        bool processCustomDice(Player& p, int x, int y);
        bool resolveDiceResult(Player& p, int d1, int d2);

        bool processJailTurn(Player& p, bool& hasUsedSkillCardThisTurn, bool& hasRolledDiceThisTurn, bool& canRollDice);
        bool processNormalTurn(Player& p, bool& hasUsedSkillCardThisTurn, bool& hasRolledDiceThisTurn, bool& canRollDice);
        void processEndTurn(Player& p); // perlu dip

        bool hasSoleWinner() const;
        void decideWinner() const;

        void showInventory(Player& p); // perlu dipindahin si
        void showPosition(Player& p); // perlu dipindahin si

        void handleSkillCardUsage(Player& p, SkillCard& card);
        void executeMoveCard(Player& p, MoveCard& card);
        void executeTeleportCard(Player& p);
        void executeLassoCard(Player& p);

        void addTransactionLog(const std::string& username, const std::string& action, const std::string& detail);
        void printTransactionLog(int lastN = -1);
        
        void saveGameState(const std::string& filename);
        void initializeWriter(const std::string& saveFolder);
};

