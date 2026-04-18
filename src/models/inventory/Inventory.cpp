#include "../../../include/models/inventory/Inventory.hpp"

Inventory::Inventory() {}

Inventory::~Inventory() {}

std::vector<PropertyTile*> Inventory::getProperties() {
    return properties_;
}
void Inventory::addProperty(PropertyTile *propertyTile) {
    properties_.push_back(propertyTile);
}

// better make id seperinya tp lg malas hehe
void Inventory::removeProperty(PropertyTile *propertyTile) {
    for (size_t i = 0; i < properties_.size(); i++) {
        if (propertyTile == properties_[i]) {
            properties_.erase(properties_.begin() + i);
            return;
        }
    }
    
}

std::vector<SkillCard*> Inventory::getSkillCards() {
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
