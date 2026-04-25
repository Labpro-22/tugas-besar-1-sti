#include "controllers/GameConfig.hpp"

int GameConfig::getMaxTurn(){
    return MAX_TURN;
}
int GameConfig::getStartingBalance(){
    return STARTING_BALANCE;
}
int GameConfig::getGoSalary(){
    return GO_SALARY;
}
int GameConfig::getJailFine(){
    return JAIL_FINE;
}

void GameConfig::setMaxTurn(int maxTurn) {
    MAX_TURN = maxTurn;
}

void GameConfig::setStartingBalance(int startingBalance) {
    STARTING_BALANCE = startingBalance;
}

void GameConfig::setGoSalary(int goSalary) {
    GO_SALARY = goSalary;
}

void GameConfig::setJailFine(int jailFine) {
    JAIL_FINE = jailFine;
}