#include "core/Monopoly.hpp"
#include "models/board/Board.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/tile/property_tile/StreetTile.hpp"
#include "models/tile/property_tile/UtilityTile.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

class CLIView : public GameViewInterface {
    private:
        Board* board_ = nullptr;

        static std::string propertyStatusToString(PropertyStatus status) {
            switch (status) {
                case PropertyStatus::BANK: return "BANK";
                case PropertyStatus::OWNED: return "OWNED";
                case PropertyStatus::MORTGAGED: return "MORTGAGED";
                default: return "UNKNOWN";
            }
        }

        static std::string propertyKind(const PropertyTile& property) {
            if (dynamic_cast<const StreetTile*>(&property) != nullptr) {
                return "Street";
            }
            if (dynamic_cast<const RailRoadTile*>(&property) != nullptr) {
                return "RailRoad";
            }
            if (dynamic_cast<const UtilityTile*>(&property) != nullptr) {
                return "Utility";
            }
            return "Property";
        }

        int boardSize() const {
            if (board_ == nullptr) {
                return 0;
            }

            int size = 0;
            for (;;) {
                try {
                    (void)board_->getCurrentTile(size);
                    ++size;
                } catch (const std::exception&) {
                    break;
                } catch (...) {
                    break;
                }
            }
            return size;
        }

    public:
        void showMessage(std::string message) override {
            std::cout << message;
        }

        void setBoardContext(Board* board) override {
            board_ = board;
        }

        void cetakPapan() override {
            if (board_ == nullptr) {
                std::cout << "Board belum tersedia.\n";
                return;
            }

            const int n = boardSize();
            if (n == 0) {
                std::cout << "Board kosong.\n";
                return;
            }

            std::cout << "===== BOARD =====\n";
            for (int i = 0; i < n; ++i) {
                Tile& tile = board_->getCurrentTile(i);
                PropertyTile* property = dynamic_cast<PropertyTile*>(&tile);

                std::cout << "[" << i << "] "
                          << tile.getLetterCode() << " - "
                          << tile.getTileName();

                if (property != nullptr) {
                    std::cout << " | " << propertyKind(*property)
                              << " | Owner: " << property->getOwnerUsername()
                              << " | Status: " << propertyStatusToString(property->getPropertyStatus());
                }

                std::cout << "\n";
            }
        }

        void cetakAkta() override {
            if (board_ == nullptr) {
                std::cout << "Board belum tersedia.\n";
                return;
            }

            const int n = boardSize();
            if (n == 0) {
                std::cout << "Board kosong.\n";
                return;
            }

            std::cout << "===== AKTA PROPERTI =====\n";
            for (int i = 0; i < n; ++i) {
                Tile& tile = board_->getCurrentTile(i);
                PropertyTile* property = dynamic_cast<PropertyTile*>(&tile);
                if (property == nullptr) {
                    continue;
                }

                std::cout << property->getLetterCode() << " | "
                          << property->getTileName() << " | "
                          << propertyKind(*property) << " | "
                          << "Harga beli: " << property->getPurchasePrice() << " | "
                          << "Nilai gadai: " << property->getMortgageValue() << " | "
                          << "Level: " << property->getLevel() << " | "
                          << "Owner: " << property->getOwnerUsername() << " | "
                          << "Status: " << propertyStatusToString(property->getPropertyStatus())
                          << "\n";
            }
        }

        void cetakProperti() override {
            if (board_ == nullptr) {
                std::cout << "Board belum tersedia.\n";
                return;
            }

            const int n = boardSize();
            if (n == 0) {
                std::cout << "Board kosong.\n";
                return;
            }

            std::cout << "===== DAFTAR PROPERTI DAN PEMILIK =====\n";
            for (int i = 0; i < n; ++i) {
                Tile& tile = board_->getCurrentTile(i);
                PropertyTile* property = dynamic_cast<PropertyTile*>(&tile);
                if (property == nullptr) {
                    continue;
                }

                std::cout << property->getLetterCode() << " - "
                          << property->getTileName() << " | "
                          << propertyKind(*property) << " | "
                          << "Owner: " << property->getOwnerUsername() << "\n";
            }
        }
};

class CLICommand : public CommandInterface {
    private:
        static std::string toUpper(std::string s) {
            std::transform(s.begin(), s.end(), s.begin(),
                [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
            return s;
        }

        static bool askYesNo(const std::string& prompt) {
            while (true) {
                std::cout << prompt << " (y/n): ";
                std::string input;
                std::getline(std::cin, input);
                input = toUpper(input);

                if (input == "Y" || input == "YES") {
                    return true;
                }
                if (input == "N" || input == "NO") {
                    return false;
                }

                std::cout << "Input tidak valid. Masukkan y atau n.\n";
            }
        }

        static bool parseInt(const std::string& token, int& value) {
            std::istringstream iss(token);
            iss >> value;
            return !iss.fail() && iss.eof();
        }

    public:
        Command getCommand() override {
            std::string line;
            std::getline(std::cin, line);

            std::istringstream iss(line);
            std::string rawCmd;
            iss >> rawCmd;

            if (rawCmd.empty()) {
                return Command(CommandType::END_COMMAND);
            }

            std::string cmd = toUpper(rawCmd);

            if (cmd == "CETAK_PAPAN") return Command(CommandType::CETAK_PAPAN);
            if (cmd == "LEMPAR_DADU") return Command(CommandType::LEMPAR_DADU);
            if (cmd == "ATUR_DADU") {
                std::vector<int> args;
                std::string token;
                while (iss >> token) {
                    int value = 0;
                    if (parseInt(token, value)) {
                        args.push_back(value);
                    }
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
            if (cmd == "CETAK_LOG") return Command(CommandType::CETAK_LOG);
            if (cmd == "GUNAKAN_KEMAMPUAN") return Command(CommandType::GUNAKAN_KEMAMPUAN);
            if (cmd == "BAYAR_DENDA") return Command(CommandType::BAYAR_DENDA);
            if (cmd == "END_COMMAND" || cmd == "END_TURN") return Command(CommandType::END_COMMAND);
            if (cmd == "INVENTORY") return Command(CommandType::INVENTORY);
            if (cmd == "POSITION") return Command(CommandType::POSITION);

            return Command(CommandType::END_COMMAND);
        }

        int getInt(int lowerBound, int upperBound) override {
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

        int getAuctionBidOrPass(int currentBid, int playerBalance) override {
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

        bool askWantToSellAllBuildings(std::string question) override {
            return askYesNo(question);
        }

        std::string askFolderForConfig() override {
            std::cout << "Masukkan folder config (default: config/initial): ";
            std::string folder;
            std::getline(std::cin, folder);
            if (folder.empty()) {
                return "config/initial";
            }
            return folder;
        }

        int askNumOfPlayer() override {
            return getInt(2, 4);
        }

        std::string askPlayerUsername() override {
            std::cout << "Masukkan username pemain: ";
            std::string username;
            std::getline(std::cin, username);
            if (username.empty()) {
                return "Player";
            }
            return username;
        }

        bool askWantToLoadState() override {
            return askYesNo("Muat savegame?");
        }

        std::string askStateFilename() override {
            std::cout << "Masukkan path save file: ";
            std::string path;
            std::getline(std::cin, path);
            return path;
        }

        std::string getTileToGetFestival() override {
            std::cout << "Masukkan kode tile untuk festival: ";
            std::string tile;
            std::getline(std::cin, tile);
            return tile;
        }

        bool askWantToBuyProperty() override {
            return askYesNo("Beli properti ini??");
        }

        bool getBool(std::string message) override {
            return askYesNo(message);
        }
};

int main() {
    std::unique_ptr<GameViewInterface> view = std::make_unique<CLIView>();
    std::unique_ptr<CommandInterface> command = std::make_unique<CLICommand>();

    Monopoly::setInitialBalance(1500);
    Monopoly::setMaxTurn(4);

    Monopoly game(std::move(view), std::move(command));
    game.startGame();
    return 0;
}