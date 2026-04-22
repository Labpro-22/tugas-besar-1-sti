#include "../../../include/models/inventory/Inventory.hpp"

Inventory::Inventory() {}

Inventory::~Inventory() {}

std::vector<PropertyTile*> Inventory::getProperties() const {
    return properties_;
}
void Inventory::addProperty(PropertyTile *propertyTile) {
    if (propertyTile == nullptr) return;

    if (!hasProperty(propertyTile)) {
        properties_.push_back(propertyTile);
    }
}

// better make id seperinya tp lg malas hehe
void Inventory::removeProperty(PropertyTile *propertyTile) {
    if (propertyTile == nullptr) return;
    for (size_t i = 0; i < properties_.size(); i++) {
        if (propertyTile->getTileID() == properties_[i]->getTileID()) {
            properties_.erase(properties_.begin() + i);
            return;
        }
    }
    
}

bool Inventory::hasProperty(PropertyTile* propertyTile) const {
    if (propertyTile == nullptr) return false;

    for (size_t i = 0; i < properties_.size(); i++) {
        if (properties_[i]->getTileID() == propertyTile->getTileID()) {
            return true;
        }
    }
    return false;
}

void Inventory::clearProperties() {
    properties_.clear();
}

std::vector<SkillCard*> Inventory::getSkillCards() const {
    return skillCards_;
}


// perlu exception??
void Inventory::addSkillCards(SkillCard* skillCard) {
    if (skillCards_.size() >= 3) {
        // throw exception??
    }

    skillCards_.push_back(skillCard);
}

void Inventory::removeSkillCard(SkillCard* skillCard) {
    for (size_t i = 0; i < skillCards_.size(); i++) {
        if (skillCard == skillCards_[i]) {
            skillCards_.erase(skillCards_.begin() + i);
            return;
        }
    }
}

int Inventory::countAllPropertyValueBasedOnPurchasePrice() const {
    int sum = 0;
    for (size_t i = 0; i < properties_.size(); i++) {
        sum += properties_.at(i)->getPurchasePrice();
    }
    return sum;
    
}

int Inventory::countAllBuildingsBasedOnPurchasePrice() const {
    int sum = 0;
    for (size_t i = 0; i < properties_.size(); i++) {
        StreetTile* s = dynamic_cast<StreetTile*>(properties_.at(i));
        if (s) {
            int level = properties_.at(i)->getLevel();
            std::map<int, int> buildingPrice = s->getBuildPrice();
            for (size_t lev = 0; lev <= level; lev++) {
                sum += buildingPrice.at(lev);
            }
        }
    }
    return sum;
}