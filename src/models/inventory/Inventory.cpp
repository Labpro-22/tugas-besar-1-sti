#include "../../../include/models/inventory/Inventory.hpp"

Inventory::Inventory() {}

Inventory::~Inventory() {}

std::vector<std::reference_wrapper<PropertyTile>> Inventory::getProperties() {
    return properties_;
}
void Inventory::addProperty(PropertyTile& propertyTile) {
    properties_.push_back(propertyTile);
}

// better make id seperinya tp lg malas hehe
void Inventory::removeProperty(const PropertyTile& propertyTile) {
    for (size_t i = 0; i < properties_.size(); i++) {
        if (propertyTile.getTileID() == properties_[i].get().getTileID()) {
            properties_.erase(properties_.begin() + i);
            return;
        }
    }
    
}

const std::vector<std::unique_ptr<SkillCard>>& Inventory::getSkillCards() const {
    return skillCards_;
}


// perlu exception??
void Inventory::addSkillCards(std::unique_ptr<SkillCard> skillCard) {
    if (skillCards_.size() >= 3) {
        // throw exception??
    }

    skillCards_.push_back(std::move(skillCard));
}

void Inventory::removeSkillCard(const SkillCard& skillCard) {
    for (size_t i = 0; i < skillCards_.size(); i++) {
        if (skillCards_[i].get() == &skillCard) {
            skillCards_.erase(skillCards_.begin() + i);
            return;
        }
    }
}

int Inventory::countAllPropertyValueBasedOnPurchasePrice() const {
    int sum = 0;
    for (size_t i = 0; i < properties_.size(); i++) {
        sum += properties_.at(i).get().getPurchasePrice();
    }
    return sum;
    
}

int Inventory::countAllBuildingsBasedOnPurchasePrice() const {
    int sum = 0;
    for (size_t i = 0; i < properties_.size(); i++) {
        PropertyTile& property = properties_.at(i).get();
        StreetTile* s = dynamic_cast<StreetTile*>(&property);
        if (s) {
            int level = property.getLevel();
            std::map<int, int> buildingPrice = s->getBuildPrice();
            for (size_t lev = 0; lev <= level; lev++) {
                sum += buildingPrice.at(lev);
            }
        }
    }
    return sum;
}

std::size_t Inventory::getSkillCardCount() const {
    return skillCards_.size();
}

const SkillCard* Inventory::getSkillCardAt(std::size_t idx) const {
    if (idx >= skillCards_.size()) {
        // TODO
        //throw OutOfRangeException;
    }
    return skillCards_.at(idx).get();
}

std::unique_ptr<SkillCard> Inventory::takeSkillCard(std::size_t idx) {
    if (idx >= skillCards_.size()) {
        // TODO
        //throw OutOfRangeException;
    }

    std::unique_ptr<SkillCard> card = std::move(skillCards_.at(idx));
    skillCards_.erase(skillCards_.begin() + static_cast<std::ptrdiff_t>(idx));
    return card;
}