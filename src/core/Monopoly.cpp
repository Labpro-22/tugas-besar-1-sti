#include "core/Monopoly.hpp";
Monopoly::Monopoly(std::unique_ptr<GameViewInterface> view,
    std::unique_ptr<CommandInterface> command)
    : view_(std::move(view)), command_(std::move(command)) {}

void Monopoly::setMaxTurn(int maxTurn) {
    maxTurn_ = maxTurn;
}

void Monopoly::setInitialBalance(int initialBalance) {
    initialBalance_ = initialBalance;
}

void Monopoly::startGame() {
    while (true) {
        view_->showMessage("Selamat datang!\nPutra-putri terbaik bangsa!");
        // minta folder yang mau di-load
        std::unique_ptr<Board> board = loadConfig();

        // tanyain apakah mulai game baru atau yang udah ada
            // if udah ada artinya minta game state-nya

        int numOfPlayers;
        int latestTurn = 0;
        std::vector<std::unique_ptr<Player>> players;
        if (command_->askWantToLoadState()) {
            // Load state
                // Bangun board
                // Bangun player
                // Cari latest turn-nya berapa
        } else {
            // new game
            numOfPlayers = command_->askNumOfPlayer();
            players = registerPlayers(numOfPlayers);
            shufflePlayersTurn(players);
        }

        // init dice
        Dice dice = Dice();

        // init deck
        Deck<SkillCard> decks = Deck<SkillCard>();

        // tar
        GameController gameController(players, *board, dice, *view_, *command_, decks); // tar lg pusing command sm si view bedanya apa
        gameController.playGame(latestTurn, maxTurn_);

        // clean log
    }
}

std::vector<std::unique_ptr<Player>> Monopoly::registerPlayers(int numOfPlayers) {
    // register minta nama dll
    if (numOfPlayers < 2) {
        // throw exception
    }
    std::vector<std::unique_ptr<Player>> players_;
    for (size_t i = 0; i < numOfPlayers; i++) {
        std::string usn = command_->askPlayerUsername();
        players_.push_back(std::make_unique<Player>(usn, initialBalance_));
    }
    return players_;
}

void Monopoly::shufflePlayersTurn(std::vector<std::unique_ptr<Player>>& players) {
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(players.begin(), players.end(), g);

    for (size_t i= 0; i < players.size(); i++) {
        players.at(i)->setTurn(i);
    }
}

std::unique_ptr<Board> Monopoly::loadConfig() {
    std::string folderName = command_->askFolderForConfig();

    // proses sampe ga nil si ??
}

// LoadState();