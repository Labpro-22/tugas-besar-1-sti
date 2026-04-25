#include "core/Monopoly.hpp"
#include "models/board/Board.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/tile/property_tile/StreetTile.hpp"
#include "models/tile/property_tile/UtilityTile.hpp"
#include "controllers/GameConfig.hpp"
#include "models/player/Player.hpp"

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
            if (boardSize() < 40) {
                std::cout << "Board tidak lengkap (butuh 40 petak).\n";
                return;
            }

            const int W = 14; 

            auto getPColor = [&](int id) {
                if (id == 1) return "\033[38;5;226m";
                if (id == 2) return "\033[38;5;51m";
                if (id == 3) return "\033[38;5;201m";
                if (id == 4) return "\033[38;5;82m";
                return "\033[0m";
            };

            auto getColorData = [&](int idx) -> std::pair<std::string, std::string> {
                if (idx==1||idx==3) return {"\033[38;5;94m", "[CK]"}; 
                if (idx==6||idx==8||idx==9) return {"\033[96m", "[BM]"}; 
                if (idx==11||idx==13||idx==14) return {"\033[95m", "[PK]"}; 
                if (idx==16||idx==18||idx==19) return {"\033[38;5;208m", "[OR]"}; 
                if (idx==21||idx==23||idx==24) return {"\033[91m", "[MR]"}; 
                if (idx==26||idx==27||idx==29) return {"\033[93m", "[KN]"}; 
                if (idx==31||idx==32||idx==34) return {"\033[92m", "[HJ]"}; 
                if (idx==37||idx==39) return {"\033[34m", "[BT]"}; 
                if (idx==12||idx==28) return {"\033[37m", "[AB]"}; 
                return {"\033[0m", "[DF]"}; 
            };

            auto getL1 = [&](int idx) {
                Tile& t = board_->getCurrentTile(idx);
                auto color = getColorData(idx);
                std::string rawStr = color.second + " " + t.getLetterCode();
                
                int spaces = W - rawStr.length();
                if (spaces < 0) spaces = 0;
                return color.first + rawStr + "\033[0m" + std::string(spaces, ' '); 
            };

            auto getL2 = [&](int idx) {
                std::string visStr = ""; 
                std::string colStr = ""; 
                
                Tile& t = board_->getCurrentTile(idx);
                int ownerNum = -1;
                
                PropertyTile* p = dynamic_cast<PropertyTile*>(&t);
                if (p) {
                    std::string owner = p->getOwnerUsername();
                    if (owner != "BANK" && !owner.empty()) {
                        for (Player* player : Player::getAllPlayersStatic()) {
                            if (player != nullptr && player->getUsername() == owner) {
                                ownerNum = player->getID() + 1; 
                                break;
                            }
                        }
                        
                        std::string ownVis = (ownerNum != -1) ? ("P" + std::to_string(ownerNum)) : owner.substr(0,2); 
                        StreetTile* s = dynamic_cast<StreetTile*>(p);
                        if (s) {
                            int level = s->getLevel();
                            if (level == 1) ownVis += " ^";
                            else if (level == 2) ownVis += " ^^";
                            else if (level == 3) ownVis += " ^^^";
                            else if (level == 4) ownVis += " *"; 
                        }
                        
                        visStr += ownVis;
                        if (ownerNum != -1) colStr += getPColor(ownerNum) + ownVis + "\033[0m";
                        else colStr += ownVis;
                    }
                }

                std::string inVis = "", inCol = "";
                std::string vVis = "", vCol = "";
                std::string normVis = "", normCol = "";

                for (Player* player : Player::getAllPlayersStatic()) {
                    if (player == nullptr) continue;
                    
                    int pIdx = player->getPosition();
                    if (pIdx > 0) pIdx--; 
                    if (player->getPosition() == 0) pIdx = 0; 

                    if (pIdx == idx) {
                        int pID = player->getID() + 1;
                        std::string pNum = std::to_string(pID);
                        std::string pColorFull = getPColor(pID) + pNum + "\033[0m";

                        if (idx == 10) {
                            bool isJailed = player->isInJail(); 
                            
                            if (isJailed) {
                                inVis += pNum;
                                inCol += pColorFull;
                            } else {
                                vVis += pNum;
                                vCol += pColorFull;
                            }
                        } else {
                            normVis += pNum;
                            normCol += pColorFull;
                        }
                    }
                }

                std::string tokenVis = "";
                std::string tokenCol = "";

                if (idx == 10) {
                    if (!inVis.empty() || !vVis.empty()) {
                        std::string inPartVis = inVis.empty() ? "" : "IN:" + inVis;
                        std::string inPartCol = inCol.empty() ? "" : "IN:" + inCol;
                        std::string vPartVis = vVis.empty() ? "" : "V:" + vVis;
                        std::string vPartCol = vCol.empty() ? "" : "V:" + vCol;
                        
                        std::string midSpc = (!inVis.empty() && !vVis.empty()) ? " " : "";
                        
                        tokenVis = "(" + inPartVis + midSpc + vPartVis + ")";
                        tokenCol = "(" + inPartCol + midSpc + vPartCol + ")";
                    }
                } else {
                    if (!normVis.empty()) {
                        tokenVis = "(" + normVis + ")";
                        tokenCol = "(" + normCol + ")";
                    }
                }

                if (!tokenVis.empty()) {
                    if (!visStr.empty()) {
                        visStr += " ";
                        colStr += " ";
                    }
                    visStr += tokenVis;
                    colStr += tokenCol;
                }

                int spaces = W - visStr.length();
                if (spaces < 0) spaces = 0;
                
                return colStr + std::string(spaces, ' ');
            };

            int midSpace = 9 * (W + 1) - 1; 
            std::vector<std::string> centerLines(26, std::string(midSpace, ' '));
            
            auto setMid = [&](int idx, std::string text) {
                int pLeft = (midSpace - text.length()) / 2;
                int pRight = midSpace - text.length() - pLeft;
                centerLines[idx] = std::string(pLeft, ' ') + text + std::string(pRight, ' ');
            };

            setMid(1, "==========================================");
            setMid(2, "||              NIMONSPOLI              ||");
            setMid(3, "==========================================");
            
            // TODO: Gimana caranya buat Turn yah
            std::string turnInfo = "TURN 15 / " + std::to_string(Monopoly::getMaxTurn());
            setMid(5, turnInfo); 

            setMid(8, "-----------------------------------------");
            setMid(9, "LEGENDA KEPEMILIKAN & STATUS              ");
            setMid(10, "P1-P4 : Properti milik Pemain 1-4        ");
            setMid(11, "^     : Rumah Level 1                    ");
            setMid(12, "^^    : Rumah Level 2                    ");
            setMid(13, "^^^   : Rumah Level 3                    ");
            setMid(14, "* : Hotel (Maksimal)                     ");
            setMid(15, "(1)-(4): Bidak (IN=Tahanan, V=Mampir)    ");
            setMid(16, "-----------------------------------------");
            setMid(17, "KODE WARNA:                              ");
            setMid(18, "[CK]=Coklat    [MR]=Merah                ");
            setMid(19, "[BM]=Biru Muda [KN]=Kuning               ");
            setMid(20, "[PK]=Pink      [HJ]=Hijau                ");
            setMid(21, "[OR]=Orange    [BT]=Biru Tua             ");
            setMid(22, "[DF]=Aksi      [AB]=Utilitas             ");

            auto printFullHLine = [&]() {
                std::cout << "+";
                for(int i=0; i<11; i++) std::cout << std::string(W, '-') << "+";
                std::cout << "\n";
            };

            printFullHLine();
            std::cout << "|";
            for (int i = 20; i <= 30; i++) std::cout << getL1(i) << "|";
            std::cout << "\n|";
            for (int i = 20; i <= 30; i++) std::cout << getL2(i) << "|";
            std::cout << "\n";
            printFullHLine();

            for (int i = 0; i < 9; i++) {
                int leftIdx = 19 - i;
                int rightIdx = 31 + i;
                
                std::cout << "|" << getL1(leftIdx) << "|" << centerLines[i * 3] << "|" << getL1(rightIdx) << "|\n";
                std::cout << "|" << getL2(leftIdx) << "|" << centerLines[i * 3 + 1] << "|" << getL2(rightIdx) << "|\n";
                if (i < 8) {
                    std::cout << "+" << std::string(W, '-') << "+" << centerLines[i * 3 + 2] << "+" << std::string(W, '-') << "+\n";
                }
            }

            printFullHLine();
            std::cout << "|";
            for (int i = 10; i >= 0; i--) std::cout << getL1(i) << "|";
            std::cout << "\n|";
            for (int i = 10; i >= 0; i--) std::cout << getL2(i) << "|";
            std::cout << "\n";
            printFullHLine();
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
            if (cmd == "END_COMMAND") return Command(CommandType::END_COMMAND);

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