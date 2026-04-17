#pragma once
#include <map>
#include "RailRoadTile.hpp"
class UtilityTile : public PropertyTile {
    private:
        static inline std::map<int, int> utilityFactor_;
    public:
        UtilityTile();
        virtual ~UtilityTile();
};
