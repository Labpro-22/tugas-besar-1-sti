#include "core/Monopoly.hpp"
#include "controllers/GameConfig.hpp"
#include "models/io/ConfigReader.hpp"
#include "models/card/skillcard/MoveCard.hpp"
#include "models/card/skillcard/DiscountCard.hpp"
#include "models/card/skillcard/LassoCard.hpp"
#include "models/card/skillcard/ShieldCard.hpp"
#include "models/card/skillcard/TeleportCard.hpp"
#include "models/card/skillcard/DemolitionCard.hpp"
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

Monopoly::Monopoly(std::unique_ptr<GameViewInterface> view,
    std::unique_ptr<CommandInterface> command)
    : view_(std::move(view)), command_(std::move(command)) {}

void Monopoly::setMaxTurn(int maxTurn) {
    maxTurn_ = maxTurn;
}

void Monopoly::setInitialBalance(int initialBalance) {
    initialBalance_ = initialBalance;
}

int Monopoly::getMaxTurn() {
    return maxTurn_;
}

void Monopoly::startGame() {
    while (true) {
        view_->showMessage("Selamat datang!\nPutra-putri terbaik bangsa!");
        // minta folder yang mau di-load
        std::unique_ptr<Board> board = loadConfig();

        if (!board) {
            view_->showMessage("Papan gagal dimuat! Cek nama folder konfigurasi.");
            return;
        }

        view_->setBoardContext(board.get());

        // tanyain apakah mulai game baru atau yang udah ada
            // if udah ada artinya minta game state-nya

        size_t numOfPlayers = 0;
        int latestTurn = 0;
        bool loadedFromState = false;
        std::vector<std::unique_ptr<Player>> players;
        if (command_->askWantToLoadState()) {
            if (!loadState(board, players, latestTurn)) {
                view_->showMessage("Load savegame gagal. Kembali ke menu awal.\n");
                continue;
            }
            loadedFromState = true;
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
        std::vector<std::unique_ptr<SkillCard>> initialSkillCards;

        for (int i = 0; i < MoveCard::kCardCount; ++i) {
            initialSkillCards.push_back(std::make_unique<MoveCard>());
        }
        for (int i = 0; i < DiscountCard::kCardCount; ++i) {
            initialSkillCards.push_back(std::make_unique<DiscountCard>());
        }
        for (int i = 0; i < LassoCard::kCardCount; ++i) {
            initialSkillCards.push_back(std::make_unique<LassoCard>());
        }
        for (int i = 0; i < ShieldCard::kCardCount; ++i) {
            initialSkillCards.push_back(std::make_unique<ShieldCard>());
        }
        for (int i = 0; i < TeleportCard::kCardCount; ++i) {
            initialSkillCards.push_back(std::make_unique<TeleportCard>());
        }
        for (int i = 0; i < DemolitionCard::kCardCount; ++i) {
            initialSkillCards.push_back(std::make_unique<DemolitionCard>());
        }

        decks.initDeck(std::move(initialSkillCards));

        (void)loadedFromState;

        // tar
        GameController gameController(std::move(players), *board, dice, *view_, *command_, decks); // tar lg pusing command sm si view bedanya apa
        gameController.playGame(latestTurn, maxTurn_);

        // clean log
    }
}

std::vector<std::unique_ptr<Player>> Monopoly::registerPlayers(size_t numOfPlayers) {
    // register minta nama dll
    if (numOfPlayers < 2) {
        // throw exception
    }
    std::vector<std::unique_ptr<Player>> players_;
    for (size_t i = 0; i < numOfPlayers; i++) {
        while (true) {
            std::string usn = command_->askPlayerUsername();
            bool exists = false;

            for (const auto& player : players_) {
                if (player->getUsername() == usn) {
                    exists = true;
                    break;
                }
            }

            if (exists) {
                view_->showMessage("Username sudah dipakai. Masukkan username lain.\n");
                continue;
            }

            players_.push_back(std::make_unique<Player>(usn, initialBalance_));
            break;
        }
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
    std::string folder = command_->askFolderForConfig();
    try {
        Reader reader(folder);
        try {
            Monopoly::setInitialBalance(reader.getStartingBalance());
            Monopoly::setMaxTurn(reader.getMaxTurn());
        } catch (...) {
        }

        try {
            GameConfig::setStartingBalance(reader.getStartingBalance());
            GameConfig::setMaxTurn(reader.getMaxTurn());
            GameConfig::setGoSalary(reader.getGoSalary());
            GameConfig::setJailFine(reader.getJailFine());
        } catch (...) {
        }

        return std::make_unique<Board>(reader.loadBoard());
    } catch (const std::exception& e) {
        std::cout << "\nConfigReader gagal karena: " << e.what() << "\n";
        return nullptr;
    } catch (...) {
        std::cout << "\nConfigReader unknown error!\n";
        return nullptr;
    }
}

bool Monopoly::loadState(std::unique_ptr<Board>& board, std::vector<std::unique_ptr<Player>>& players, int& latestTurn) 
{
    std::string filename = command_->askStateFilename();

    std::ifstream file(filename);
    if (!file.is_open()) 
    {
        view_->showMessage("Gagal membuka file: " + filename);
        return false;
    }

    try 
    {
        std::string line;
        std::getline(file, line);
        std::istringstream issTurn(line);
        int maxTurn;
        issTurn >> latestTurn >> maxTurn;
        Monopoly::setMaxTurn(maxTurn);

        std::getline(file, line);
        int playerCount = std::stoi(line);
        players.clear();

        for (int i = 0; i < playerCount; ++i) 
        {
            std::getline(file, line);
            std::istringstream issP(line);
            std::string username, posCode, statusStr;
            int balance;
            issP >> username >> balance >> posCode >> statusStr;
            auto player = std::make_unique<Player>(username, balance);
            player->setPosition(board->getTileIndexByCode(posCode));

            if (statusStr == "BANKRUPT") {
                player->setStatus(Player::PlayerStatus::BANKRUPT);
            } else if (statusStr.rfind("JAILED", 0) == 0) {
                // status may be "JAILED" or "JAILED_X" where X is jail turn count
                player->setStatus(Player::PlayerStatus::JAILED);
                // parse optional _X
                size_t pos = statusStr.find('_');
                player->resetJailTurn();
                if (pos != std::string::npos) {
                    try {
                        int count = std::stoi(statusStr.substr(pos + 1));
                        for (int k = 0; k < count; ++k) {
                            player->incrementJailTurn();
                        }
                    } catch (...) {
                        // ignore malformed suffix
                    }
                }
            } else {
                player->setStatus(Player::PlayerStatus::ACTIVE);
            }

            std::getline(file, line);
            int cardCount = std::stoi(line);
            for (int j = 0; j < cardCount; ++j) 
            {
                std::getline(file, line);
                std::istringstream issC(line);
                std::string cardType;
                issC >> cardType;

                int value = 0, duration = 0;
                
                // TODO: Sesuaikan dengan nama class spesifik dari SkillCard kalian
                if (cardType == "MoveCard") {
                    issC >> value;
                    // player->addSkillCard(std::make_unique<MoveCard>(value));
                } else if (cardType == "DiscountCard") {
                    issC >> value >> duration;
                    // player->addSkillCard(std::make_unique<DiscountCard>(value, duration));
                } else {
                    // ShieldCard / TeleportCard
                    // player->addSkillCard(std::make_unique<ShieldCard>());
                }
            }
            players.push_back(std::move(player));
        }

        std::getline(file, line); // <URUTAN_GILIRAN_1> ...
        std::getline(file, line); // <GILIRAN_AKTIF_SAAT_INI>

        std::getline(file, line);
        int propCount = std::stoi(line);
        
        for (int i = 0; i < propCount; ++i) 
        {
            std::getline(file, line);
            std::istringstream issProp(line);
            std::string kode, jenis, pemilik, statusStr, nBangunanStr;
            int fmult, fdur;

            issProp >> kode >> jenis >> pemilik >> statusStr >> fmult >> fdur >> nBangunanStr;

            Tile& tile = board->getTileByCode(kode);
            
            PropertyTile* propTile = dynamic_cast<PropertyTile*>(&tile);
            if (propTile != nullptr) 
            {
                // Set Username
                propTile->setOwnerUsername(pemilik);

                // Set Status Properti
                if (statusStr == "OWNED") propTile->setPropertyStatus(PropertyStatus::OWNED);
                else if (statusStr == "MORTGAGED") propTile->setPropertyStatus(PropertyStatus::MORTGAGED);
                else propTile->setPropertyStatus(PropertyStatus::BANK);

                // Set Festival
                propTile->setFestivalMultiplier(fmult);
                propTile->setFestivalDuration(fdur);

                // Set Level Bangunan (Ubah "H" jadi 5)
                if (nBangunanStr == "H") {
                    propTile->setLevel(5); 
                } else {
                    propTile->setLevel(std::stoi(nBangunanStr));
                }
                // Jika properti dimiliki oleh pemain, tambahkan ke inventory pemain
                // PLIS JANGAN DI HAPUS - DEMI LIQUIDATION PLIS 
                if (pemilik != "BANK") {
                    for (auto& pl : players) {
                        if (pl->getUsername() == pemilik) {
                            pl->addProperty(propTile);
                            break;
                        }
                    }
                }
            }
        }

        // 5. BACA STATE DECK & LOG
        // ... Logika baca Deck ...

        view_->showMessage("Load State Berhasil! Melanjutkan permainan...\n");
        return true;

    } 
    catch (const std::exception& e) 
    {
        view_->showMessage("Error: Format savegame rusak atau tidak valid! (" + std::string(e.what()) + ")");
        return false;
    }
}