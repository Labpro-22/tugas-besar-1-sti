#pragma once
#include <map>
#include "PropertyTile.hpp"
class RailRoadTile : public PropertyTile {
    private:
        static inline std::map<int, int> railRoadRentPrices_;
    public:
        // perlu method static untuk set value map-nya waktu ngeload config?? :/
        RailRoadTile();
        ~RailRoadTile();
};
