#include "models/card/skillcard/DemolitionCard.hpp"
#include "models/player/Player.hpp"
#include "models/tile/property_tile/PropertyTile.hpp"

DemolitionCard::DemolitionCard()
    : SkillCard("Demolition Card", "Menghancurkan satu properti milik pemain lawan") {}

void DemolitionCard::activate(Player& owner) {
    std::vector<PropertyTile*> candidates;
    candidates.reserve(16);

    for (Player* player : owner.getAllPlayers()) {
        if (player == nullptr || player == &owner) {
            continue;
        }
        if (player->isBankrupt()) {
            continue;
        }

        std::vector<PropertyTile*> properties = player->getProperties();
        for (PropertyTile* property : properties) {
            if (property == nullptr) {
                continue;
            }
            if (property->isOwnedByBank()) {
                continue;
            }
            candidates.push_back(property);
        }
    }

    if (candidates.empty()) {
        std::cout << "[DEMOLITION] Tidak ada properti lawan yang bisa dihancurkan.\n";
        return;
    }

    std::cout << "[DEMOLITION] Pilih properti lawan yang ingin dihancurkan:\n";
    for (std::size_t i = 0; i < candidates.size(); ++i) {
        PropertyTile* property = candidates[i];
        std::cout << (i + 1) << ". "
                  << "tileID=" << property->getTileID() << " "
                  << property->getTileName() << " (" << property->getLetterCode() << ")"
                  << " owner=" << property->getOwnerUsername()
                  << " status=" << static_cast<int>(property->getPropertyStatus())
                  << " level=" << property->getLevel()
                  << "\n";
    }

    PropertyTile* target = nullptr;
    while (true) {
        int chosenTileId = -1;
        std::cout << "Masukkan tile ID target: ";
        if (!(std::cin >> chosenTileId)) {
            std::cout << "Input tidak valid. Coba lagi.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        for (PropertyTile* candidate : candidates) {
            if (candidate->getTileID() == chosenTileId) {
                target = candidate;
                break;
            }
        }

        if (target != nullptr) {
            break;
        }

        std::cout << "Tile ID tidak ditemukan pada properti lawan. Coba lagi.\n";
    }

    const std::string ownerUsername = target->getOwnerUsername();
    const PropertyStatus oldStatus = target->getPropertyStatus();

    target->setLevel(0);
    target->resetFestivalEffect();

    target->setOwnerUsername(ownerUsername);
    target->setPropertyStatus(oldStatus);

    std::cout << "[DEMOLITION] " << target->getTileName()
              << " berhasil dihancurkan\n";
}
