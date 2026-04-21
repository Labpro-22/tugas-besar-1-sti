#include "../injector/injector.hpp"
#include <random>
class Dice : public IDice {
    private:
        int die1_ = 0;
        int die2_ = 0;
    public:
        Dice();
        ~Dice() override;
        int roll() override;

        int rollSettingan (int die1, int die2) override;

        bool isDouble() override;
};