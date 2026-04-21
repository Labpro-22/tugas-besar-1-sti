#pragma once
#include "Tile.hpp"
#include <string>

class ActionTile : public Tile {
public:
    ActionTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : Tile(tileID, letterCode, tileName, colourBlock) {}
        
    virtual void executeTile(Player* p) override = 0; 
};


class CardTile : public ActionTile {
public:
    CardTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : ActionTile(tileID, letterCode, tileName, colourBlock) {}
        
    virtual void executeTile(Player* p) override = 0;
};

class CommunityChestTile : public CardTile {
public:
    CommunityChestTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : CardTile(tileID, letterCode, tileName, colourBlock) {}
        
    void executeTile(Player* p) override;
};

class ChanceTile : public CardTile {
public:
    ChanceTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : CardTile(tileID, letterCode, tileName, colourBlock) {}
        
    void executeTile(Player* p) override;
};


class FestivalTile : public ActionTile {
public:
    FestivalTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : ActionTile(tileID, letterCode, tileName, colourBlock) {}
        
    void executeTile(Player* p) override;
};


class TaxTile : public ActionTile {
protected:
public:
    TaxTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : ActionTile(tileID, letterCode, tileName, colourBlock) {}
    
    virtual void executeTile(Player* p) override = 0;
};

class IncomeTaxTile : public TaxTile {
private:
    static int flatCost_;
    static float taxPercentage_;
public:
    IncomeTaxTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : TaxTile(tileID, letterCode, tileName, colourBlock) {}
    
    static void setPPHFlatCost(int cost);
    static void setTaxPercentage(float percentage);
    int getPPHFlatCost() const;
    float getTaxPercentage() const;
    void executeTile(Player* p) override;
};

class LuxuryGoodsTaxTile : public TaxTile {
private:
    static int flatCost_;
public:
    LuxuryGoodsTaxTile(int tileID, std::string letterCode, std::string tileName, std::string colourBlock)
        : TaxTile(tileID, letterCode, tileName, colourBlock) {}
    static void setPBMFlatCost(int cost);
    int getPBMFlatCost() const;
    void executeTile(Player* p) override;
};