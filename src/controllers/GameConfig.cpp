#include "controllers/GameConfig.hpp"

int GameConfig::MAX_TURN = 0;
int GameConfig::STARTING_BALANCE = 0;
int GameConfig::GO_SALARY = 0;
int GameConfig::JAIL_FINE = 0;

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