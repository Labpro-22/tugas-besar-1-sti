#include "../../include/core/Monopoly.hpp"

Monopoly::Monopoly(){}

Monopoly::Monopoly(const Monopoly&) = delete;

Monopoly& Monopoly::Get() {
    return s_Instance;
}

void Monopoly::setMaxTurn(int maxTurn) {
    this->maxTurn_ = maxTurn;
}

void Monopoly::setInitialBalance(int initialBalance) {
    this->initialBalance_ = initialBalance;
}