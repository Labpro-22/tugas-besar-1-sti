#include "models/tile/property_tile/PropertyTile.hpp"
#include "models/card/skillcard/ShieldCard.hpp"
#include <iostream>
#include <vector>
#include "models/tile/property_tile/StreetTile.hpp"
class Inventory {
    private:
        std::vector<PropertyTile*> properties_;
        std::vector<SkillCard*> skillCards_;// tar perlu jd interface?
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
        void addSkillCards(SkillCard* killCard);
        void removeSkillCard(SkillCard* skillCard);

        int countAllPropertyValueBasedOnPurchasePrice() const;
        int countAllBuildingsBasedOnPurchasePrice() const;


};
