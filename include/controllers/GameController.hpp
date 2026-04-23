#pragma once
#include "models/board/Board.hpp"
#include "models/player/Player.hpp"
#include "views/CommandInterface.hpp"
#include "views/GameViewInterface.hpp"
#include "models/dice/Dice.hpp"
#include "models/tile/action_tile/special_tile/PrisonTile.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include <memory>
#include <vector>
#include "models/card/Deck.hpp"
#include "models/card/skillcard/SkillCard.hpp"
#include "models/card/skillcard/LassoCard.hpp"
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
        void transferProperty(Player& from, Player& to, PropertyTile& propertyTile);

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

        int goSalary_;
        int jailFine_;
};

