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

#include <memory>
#include <vector>

class AuctionCoordinator;
class SkillCardCoordinator;
class PropertyCoordinator;

class GameController {
    public:
        GameController(std::vector<std::unique_ptr<Player>> players, 
            Board& board, Dice& dice, GameViewInterface& view, 
            CommandInterface& command, Deck<SkillCard>& specialCardDeck_);
        ~GameController();

        void playGame(int latesTurn, int maxTurn);
        void processTurn(Player& p);
        void processMovement(Player& p, int firstDisplacement);
        void processSpecialCardUse(Player& p, bool& hasUsedSkillCardThisTurn);
        void processPickAndDropSpecialCard(Player& p);
        void processRollDice(Player& p);
        void processAuction(Player& p, PropertyTile& propertyTile);
        void processBankruptcyToBank(Player& p);
        bool processRandomDice(Player& p);
        bool processCustomDice(Player& p, int x, int y);
        bool resolveDiceResult(Player& p, int d1, int d2);

        void processPayRent(Player& p, Tile& currentTile);
        void processBankruptcyFlow(Player& payer, Player& owner, int rent);

        void processMortgage(Player& p);
        void processBuyBuilding(Player& p);
        void processRedeem(Player& p);
        void processJailTurn(Player& p, bool& hasUsedSkillCardThisTurn);
        void processNormalTurn(Player& p, bool& hasUsedSkillCardThisTurn);
        void processFestival(Player& p);
        void processTakeChanceCard(Player& p);
        void processTakeCommunityChest(Player& p);
        bool hasSoleWinner() const;
        void decideWinner() const;

    private:
        // Deck<SpecialCard>...
        Deck<SkillCard>& specialCardDeck_;
        Board& board_;
        Dice& dice_;
        GameViewInterface& view_;
        CommandInterface& command_;
        std::vector<std::unique_ptr<Player>> players_;
        Auction auction_;
        std::unique_ptr<AuctionCoordinator> auctionCoordinator_;
        std::unique_ptr<SkillCardCoordinator> skillCardCoordinator_;
        std::unique_ptr<PropertyCoordinator> propertyCoordinator_;

        int goSalary_;
        int jailFine_;
};

