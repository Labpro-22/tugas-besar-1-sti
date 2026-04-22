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
        std::vector<PropertyTile*> getProperties();
        void addProperty(PropertyTile *propertyTile);
        void removeProperty(PropertyTile *PropertyTile);

        // skill cards
        std::vector<SkillCard*> getSkillCards();
        void addSkillCards(SkillCard* killCard);
        void removeSkillCard(SkillCard* skillCard);

        int countAllPropertyValueBasedOnPurchasePrice() const;
        int countAllBuildingsBasedOnPurchasePrice() const;

        bool isExistsTileBasedOnCode(std::string code) const;
};
