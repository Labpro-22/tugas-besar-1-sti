#include "core/Monopoly.hpp"
#include "views/CommandInterface.hpp"
#include "views/GameViewInterface.hpp"
#include "views/cli/CLICommand.hpp"
#include "views/cli/CLIView.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
int main() {
    std::unique_ptr<GameViewInterface> view = std::make_unique<CLIView>();
    std::unique_ptr<CommandInterface> command = std::make_unique<CLICommand>();

    Monopoly game(std::move(view), std::move(command));
    game.startGame();
    return 0;
}