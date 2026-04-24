#pragma once
#include "../injector/injector.hpp"
#include <random>
class Dice {
    private:
        int die1_ = 0;
        int die2_ = 0;
    public:
        Dice();
        ~Dice();
        void roll();

        void rollSettingan (int die1, int die2);

        int getRollResult() const;

        bool isDouble();

        int getDie2() const;

        int getDie1() const;

};