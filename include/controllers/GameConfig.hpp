#pragma once 

class GameConfig {
    private:
        static int MAX_TURN;
        static int STARTING_BALANCE;
        static int GO_SALARY;
        static int JAIL_FINE;
    public:
        static int getMaxTurn();
        static int getStartingBalance();
        static int getGoSalary();
        static int getJailFine();

        static void setMaxTurn(int maxTurn);
        static void setStartingBalance(int startingBalance);
        static void setGoSalary(int goSalary);
        static void setJailFine(int jailFine);
};