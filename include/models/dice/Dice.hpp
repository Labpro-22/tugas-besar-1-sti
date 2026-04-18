#include "../injector/injector.hpp"
#include <random>
class Dice : IDice {
    private:
        int die1_ = 0;
        int die2_ = 0;
    public:
        Dice();
        ~Dice() override;
        int roll() override;

        int rollSettingan (int die1, int die2) override{
            die1_ = die1;
            die2_ = die2_;
            return die1 + die2;
        }

        bool isDouble() override;
};