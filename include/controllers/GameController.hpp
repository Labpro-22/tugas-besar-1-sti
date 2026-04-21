#include "models/board/Board.hpp"
#include "models/injector/injector.hpp"
#include "models/player/Player.hpp"
#include "views/CommandInterface.hpp"
#include "views/GameViewInterface.hpp"
#include "models/injector/injector.hpp"
#include "models/tile/action_tile/special_tile/PrisonTile.hpp"

#include <vector>
class GameController {
    public:
        enum class GameState  {
            WAITING_FOR_ROLL_DICE,
            FREE_SUDAH_ROLL_DICE,
            INJAIL,

            // ada nama lain?
            LELANG, // nanti cek lagi
            BANKRUT, // perlu state?
        };

        GameController(Board& board, IDice& dice, GameViewInterface& view, CommandInterface& command);
        ~GameController();
        void processTurn(Player& p);
        void normalTurn(Player& p, int firstDisplacement);
        void auction(Player& p);

        void chooseSpecialCard(Player& p);
        void handleRollDice(Player& p);

        void dropExtraSpecialCard(Player& p);

    private:
        Board& board_;
        IDice& dice_;
        GameViewInterface& view_;
        CommandInterface& command_;

        GameState state_;
};

