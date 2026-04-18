#pragma once
class Player;

class IDice {
    public:
        virtual ~IDice() {};
        virtual int roll() = 0;
        virtual int rollSettingan(int die1, int die2) = 0;
        virtual bool isDouble() = 0;
};

class ITile {
    public:
        virtual ~ITile() {};
        virtual void onLand(Player* player) = 0;
};