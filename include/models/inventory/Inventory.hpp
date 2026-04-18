#include "../tile/PropertyTile.hpp"
#include <iostream>
class SkillCard;

class Inventory {
    private:
        std::vector<PropertyTile*> properties_;
        std::vector<SkillCard*> skillCards_;// tar perlu jd interface?
    public:
        // init owned properties sm owned skill cards
        Inventory();
        ~Inventory();
};
