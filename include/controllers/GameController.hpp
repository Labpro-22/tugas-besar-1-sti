#include "models/board/Board.hpp"
#include "models/injector/injector.hpp"
#include "models/player/Player.hpp"
#include "views/CommandInterface.hpp"
#include "views/GameViewInterface.hpp"
#include "models/injector/injector.hpp"
#include "models/tile/action_tile/special_tile/PrisonTile.hpp"
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
            Board& board, IDice& dice, GameViewInterface& view, 
            CommandInterface& command);
        ~GameController();

        void playGame(int latesTurn, int maxTurn);
        void processTurn(Player& p);
        void normalTurn(Player& p, int firstDisplacement);
        void auction(Player& p);

        void handleRollDice(Player& p);
        bool hasSoleWinner() const;

    private:
        Board& board_;
        IDice& dice_;
        GameViewInterface& view_;
        CommandInterface& command_;
        std::vector<std::unique_ptr<Player>>& players_; // gabs pake &

        GameState state_;
};

