#include "models/dice/Dice.hpp"
Dice::Dice() {}

Dice::~Dice() {}

void Dice::roll() {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distrib(1, 6);
    die1_ = distrib(gen);
    die2_ = distrib(gen);
}

void Dice::rollSettingan (int die1, int die2) {
    die1_ = die1;
    die2_ = die2;
}

int Dice::getRollResult() const {
    return die1_ + die2_;
}

int Dice::getDie1() const{
    return die1_;
}

int Dice::getDie2() const {
    return die2_;
}

bool Dice::isDouble() {
    return die1_ == die2_ && die1_ != 0; // make sure ga default value
}

