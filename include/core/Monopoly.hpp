#pragma once

class Monopoly {
    private:
        static inline int maxTurn_;
        static inline int initialBalance_;
    public:
        Monopoly(/* args */);
        ~Monopoly();

        static void setMaxTurn(int maxTurn);
        static void setInitialBalance(int initialBalance);
};
