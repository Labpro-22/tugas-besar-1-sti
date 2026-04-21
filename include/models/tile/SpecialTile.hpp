#pragma once
#include "models/tile/ActionTile.hpp"
#include <string>

// ==========================================
class SpecialTile : public ActionTile {
public:
    SpecialTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : ActionTile(tileID, letterCode, tileName, colourBlock) {}
    
    virtual void executeTile(Player* p) override = 0;
};

class GoTile : public SpecialTile {
private:
    int salary_;
public:
    GoTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int salary)
        : SpecialTile(tileID, letterCode, tileName, colourBlock), salary_(salary) {}
    
    void executeTile(Player* p) override;
    int getSalary() const { return salary_; }
};

class PrisonTile : public SpecialTile {
private:
    int fineCost_;
public:
    PrisonTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock, int fineCost)
        : SpecialTile(tileID, letterCode, tileName, colourBlock), fineCost_(fineCost) {}
    
    void executeTile(Player* p) override;
    int tryExitPrison(Player *p);
    int getFineCost() const { return fineCost_; }
};

class FreeParkingTile : public SpecialTile {
public:
    FreeParkingTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : SpecialTile(tileID, letterCode, tileName, colourBlock) {}
    
    void executeTile(Player* p) override;
};

class GoToJailTile : public SpecialTile {
public:
    GoToJailTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : SpecialTile(tileID, letterCode, tileName, colourBlock) {}
    
    void executeTile(Player* p) override;
};