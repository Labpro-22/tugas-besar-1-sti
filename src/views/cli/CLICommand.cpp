#include "views/cli/CLICommand.hpp"

Command CLICommand::getCommand() {
    std::string line;
    std::getline(std::cin, line);

    std::istringstream iss(line);
    std::string rawCmd;
    iss >> rawCmd;

    if (rawCmd.empty()) {
        return Command(CommandType::INVALID);
    }

    std::string cmd = toUpper(rawCmd);

    if (cmd == "CETAK_PAPAN") return Command(CommandType::CETAK_PAPAN);
    if (cmd == "LEMPAR_DADU") return Command(CommandType::LEMPAR_DADU);
    if (cmd == "ATUR_DADU") {
        std::vector<int> args;
        std::string token;

        while (iss >> token) {
            int value = 0;
            if (!parseInt(token, value)) {
                return Command(CommandType::INVALID); //
            }
            args.push_back(value);
        }

        if (args.size() != 2) {
            return Command(CommandType::INVALID); //
        }
        return Command(CommandType::ATUR_DADU, args);
    }
    if (cmd == "CETAK_AKTA") return Command(CommandType::CETAK_AKTA);
    if (cmd == "CETAK_PROPERTI") return Command(CommandType::CETAK_PROPERTI);
    if (cmd == "GADAI") return Command(CommandType::GADAI);
    if (cmd == "TEBUS") return Command(CommandType::TEBUS);
    if (cmd == "BANGUN") return Command(CommandType::BANGUN);
    if (cmd == "SIMPAN") return Command(CommandType::SIMPAN);
    if (cmd == "MUAT") return Command(CommandType::MUAT);
    if (cmd == "CETAK_LOG") {
        std::vector<int> args;
        std::string token;
        while (iss >> token) {
            int value = 0;
            if (!parseInt(token, value)) {
                return Command(CommandType::INVALID);
            }
            args.push_back(value);
        }

        if (args.size() > 1) {
            return Command(CommandType::INVALID);
        }

        if (args.empty()) {
            return Command(CommandType::CETAK_LOG);
        }
        return Command(CommandType::CETAK_LOG, args);
    }
    if (cmd == "GUNAKAN_KEMAMPUAN") return Command(CommandType::GUNAKAN_KEMAMPUAN);
    if (cmd == "BAYAR_DENDA") return Command(CommandType::BAYAR_DENDA);
    if (cmd == "END_COMMAND" || cmd == "END_TURN") return Command(CommandType::END_COMMAND);
    if (cmd == "INVENTORY") return Command(CommandType::INVENTORY);
    if (cmd == "POSITION") return Command(CommandType::POSITION);

    return Command(CommandType::INVALID);
}

int CLICommand::getInt(int lowerBound, int upperBound) {
    while (true) {
        std::cout << "Masukkan angka (" << lowerBound << "-" << upperBound << "): ";
        std::string line;
        std::getline(std::cin, line);

        int value = 0;
        if (!parseInt(line, value)) {
            std::cout << "Input bukan angka valid.\n";
            continue;
        }

        if (value < lowerBound || value > upperBound) {
            std::cout << "Input di luar rentang.\n";
            continue;
        }

        return value;
    }
}

int CLICommand::getAuctionBidOrPass(int currentBid, int playerBalance) {
    while (true) {
        std::cout << "Masukkan bid (>= " << currentBid << ") atau -1 untuk pass: ";
        std::string line;
        std::getline(std::cin, line);

        int value = 0;
        if (!parseInt(line, value)) {
            std::cout << "Input bukan angka valid.\n";
            continue;
        }

        if (value == -1) {
            return -1;
        }

        if (value < currentBid || value > playerBalance) {
            std::cout << "Bid tidak valid.\n";
            continue;
        }

        return value;
    }
}

bool CLICommand::askWantToSellAllBuildings(std::string question) {
    return askYesNo(question);
}

std::string CLICommand::askFolderForConfig() {
    std::cout << "Masukkan folder config (default: config/initial): ";
    std::string folder;
    std::getline(std::cin, folder);
    if (folder.empty()) {
        return "config/initial";
    }
    return folder;
}

int CLICommand::askNumOfPlayer() {
    return getInt(2, 4);
}

std::string CLICommand::askPlayerUsername() {
    std::cout << "Masukkan username pemain: ";
    std::string username;
    std::getline(std::cin, username);
    if (username.empty()) {
        return "Player";
    }
    return username;
}

bool CLICommand::askWantToLoadState() {
    return askYesNo("Muat savegame?");
}

std::string CLICommand::getTileToGetFestival()  {
    std::cout << "Masukkan kode tile untuk festival: ";
    std::string tile;
    std::getline(std::cin, tile);
    return tile;
}

bool CLICommand::askWantToBuyProperty() {
    return askYesNo("Beli properti ini??");
}

bool CLICommand::getBool(std::string message) {
    return askYesNo(message);
}

std::string CLICommand::getString() {
    std::string input;
    std::cin >> input;
    
    if (std::cin.fail()) {
        std::cin.clear();
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    
    return input;
}

std::string CLICommand::askStateFilename() {
    std::cout << "Masukkan path save file: ";
    std::string path;
    std::getline(std::cin, path);
    return path;
}