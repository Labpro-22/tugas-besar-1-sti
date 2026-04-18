#include "../tile/PropertyTile.hpp"
#include <iostream>
#include <vector>
class SkillCard;

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
        
        // kasus kalo make gimana nntlah

};
