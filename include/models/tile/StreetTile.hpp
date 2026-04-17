#pragma once
#include <map>
#include "PropertyTile.hpp"
class StreetTile : public PropertyTile {
    private:
        // color group (khusus Street) perlu?? atau cukup colorCode
        std::map<int, int> buildPrice_; //  serta harga bangunan per rumah dan hotel

    public:
        StreetTile();
        ~StreetTile();
};
