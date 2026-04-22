#pragma once
#include "SpecialTile.hpp"
#include <string>

class GoTile : public SpecialTile {
    private:
        int salary_;
    public:
        GoTile(int tileID, std::string letterCode, std::string tileName, int salary)
            : SpecialTile(tileID, letterCode, tileName), salary_(salary) {}
        
        OnLandResult onLand(Player& p, CommandInterface& command, GameViewInterface& view) override;
        int getSalary() const { return salary_; }
};