#include "../../../include/models/inventory/Inventory.hpp"

Inventory::Inventory() {}

Inventory::~Inventory() {}

std::vector<PropertyTile*> Inventory::getProperties() const {
    return properties_;
}

std::vector<PropertyTile*> Inventory::getMortgagedProperties() const {
    std::vector<PropertyTile*> mortgagedProps;
    for (size_t i = 0; i < getProperties().size(); i++) {
        if (getProperties().at(i) != nullptr && getProperties().at(i)->isMortgaged()) {
            mortgagedProps.push_back(getProperties().at(i));
        }
    }
    return mortgagedProps;
}

void Inventory::addProperty(PropertyTile *propertyTile) {
    if (propertyTile == nullptr) return;

    if (!hasProperty(propertyTile)) {
        properties_.push_back(propertyTile);
        if (dynamic_cast<RailRoadTile*>(propertyTile) != nullptr) {
            countRailRoads_++;
        }
        if (dynamic_cast<PropertyTile*>(propertyTile) != nullptr) {
            countUtilities_++;
        }
    }
}

// better make id seperinya tp lg malas hehe
void Inventory::removeProperty(PropertyTile *propertyTile) {
    if (propertyTile == nullptr) return;
    for (size_t i = 0; i < properties_.size(); i++) {
        if (properties_[i] == propertyTile) {
            properties_.erase(properties_.begin() + i);
            if (dynamic_cast<RailRoadTile*>(propertyTile) != nullptr) {
                countRailRoads_--;
            }
            if (dynamic_cast<PropertyTile*>(propertyTile) != nullptr) {
                countUtilities_--;
            }
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
    std::vector<SkillCard*> out;
    out.reserve(skillCards_.size());
    for (const auto& uptr : skillCards_) {
        out.push_back(uptr.get());
    }
    return out;
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
        sum += properties_.at(i)->getPurchasePrice();
    }
    return sum;
}

int Inventory::countAllBuildingsBasedOnPurchasePrice() const {
    int sum = 0;
    for (size_t i = 0; i < properties_.size(); i++) {
        PropertyTile* property = properties_.at(i);
        StreetTile* s = dynamic_cast<StreetTile*>(property);
        if (s) {
            int level = property->getLevel();
            std::map<int, int> buildingPrice = s->getBuildPrice();
            for (int lev = 0; lev <= level; lev++) {
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
        return nullptr;
    }
    return skillCards_.at(idx).get();
}

std::unique_ptr<SkillCard> Inventory::removeSkillCardAt(std::size_t idx) {
    if (idx >= skillCards_.size()) {
        // TODO
        //throw OutOfRangeException;
        return nullptr;
    }

    std::unique_ptr<SkillCard> card = std::move(skillCards_.at(idx));
    skillCards_.erase(skillCards_.begin() + static_cast<std::ptrdiff_t>(idx));
    return card;
}

bool Inventory::isExistsTileBasedOnCode(std::string code) const {
    for (PropertyTile* p : properties_) {
        if (p != nullptr && p->getLetterCode() == code) {
            return true;
        }
    }
    return false;
}

int Inventory::countRailRoads() const {
    return countRailRoads_;
}

int Inventory::countUtilities() const {
    return countUtilities_;
}

int Inventory::countProperty() const {
    return static_cast<int>(properties_.size());
}

int Inventory::countCard() const {
    return static_cast<int>(skillCards_.size());
}