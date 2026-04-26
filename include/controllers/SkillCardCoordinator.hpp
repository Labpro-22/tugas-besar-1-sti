#pragma once

#include "models/card/Deck.hpp"
#include "models/card/skillcard/LassoCard.hpp"
#include "models/card/skillcard/MoveCard.hpp"
#include "models/card/skillcard/SkillCard.hpp"
#include "models/card/skillcard/TeleportCard.hpp"
#include "models/card/skillcard/ShieldCard.hpp"
#include "models/card/skillcard/DemolitionCard.hpp"
#include "models/card/skillcard/DiscountCard.hpp"
#include "models/player/Player.hpp"
#include "views/CommandInterface.hpp"
#include "views/GameViewInterface.hpp"

class SkillCardCoordinator {
    public:
        SkillCardCoordinator(Deck<SkillCard>& skillCardDeck,
                            GameViewInterface& view,
                            CommandInterface& command);

        void processPickAndDropSkillCard(Player& player);
        std::unique_ptr<SkillCard> processSkillCardUse(Player& player, bool& hasUsedSkillCardThisTurn);

    private:
        Deck<SkillCard>& skillCardDeck_;
        GameViewInterface& view_;
        CommandInterface& command_;
};
