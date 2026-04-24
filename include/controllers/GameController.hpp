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

class GameController {
    public:
        enum class GameState  {
            WAITING_FOR_ROLL_DICE,
            FREE_SUDAH_ROLL_DICE,
            INJAIL,

            LELANG,
            BANKRUT,
        };

        GameController(std::vector<std::unique_ptr<Player>>& players,
            Board& board, Dice& dice, GameViewInterface& view, CommandInterface& command);
            
        ~GameController();

        void playGame(int latesTurn, int maxTurn);
        void processTurn(Player& p);
        void processMovement(Player& p, int firstDisplacement);
        void processSpecialCardUse(Player& p, bool& hasUsedSkillCardThisTurn);
        void processPickAndDropSpecialCard(Player& p);
        void processRollDice(Player& p);

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
        std::vector<std::unique_ptr<Player>>& players_; // gabs pake &
        Auction auction_;

        int goSalary_;
        int jailFine_;
};

