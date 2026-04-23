#pragma once
#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/card/skillcard/ShieldCard.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include "models/tile/property_tile/StreetTile.hpp"
#include "models/tile/property_tile/RailRoadTile.hpp"
#include "models/tile/property_tile/UtilityTile.hpp"

class Inventory {
    private:
        std::vector<PropertyTile*> properties_;
        std::vector<std::unique_ptr<SkillCard>> skillCards_;// tar perlu jd interface?
        int countUtilities_;
        int countRailRoads_;
    public:
        // init owned properties sm owned skill cards
        Inventory();
        ~Inventory();

        // properties
        std::vector<PropertyTile*> getProperties() const;
        void addProperty(PropertyTile *propertyTile);
        void removeProperty(PropertyTile *PropertyTile);
        bool hasProperty(PropertyTile* propertyTile) const;
        void clearProperties();

        // skill cards
        std::vector<SkillCard*> getSkillCards() const;
        void addSkillCards(std::unique_ptr<SkillCard> skillCard);
        void removeSkillCard(const SkillCard& skillCard);
        std::size_t getSkillCardCount() const;
        const SkillCard* getSkillCardAt(std::size_t idx) const;
        std::unique_ptr<SkillCard> removeSkillCardAt(std::size_t idx);

        int countAllPropertyValueBasedOnPurchasePrice() const;
        int countAllBuildingsBasedOnPurchasePrice() const;

        bool isExistsTileBasedOnCode(std::string code) const;

        int countRailRoads() const;
        int countUtilities() const;
};
