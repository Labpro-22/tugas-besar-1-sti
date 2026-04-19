#include "models/card/DiscountCard.hpp"

#include <iostream>
#include <random>

DiscountCard::DiscountCard()
    : SkillCard("Discount Card", "Memberikan diskon acak selama 1 giliran"),
      discountPercent_(10),
      remainingTurns_(1) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(6, 67);
    discountPercent_ = dist(gen);
}

void DiscountCard::activate(Player& owner)
{
    (void)owner;
    remainingTurns_ = 1;
    std::cout << "[DISCOUNT] Diskon " << discountPercent_
              << "% aktif untuk 1 giliran berikutnya.\n";
}

int DiscountCard::getDiscountPercent() const
{
    return discountPercent_;
}

int DiscountCard::getRemainingTurns() const
{
    return remainingTurns_;
}

void DiscountCard::consumeTurn()
{
    if (remainingTurns_ > 0)
    {
        --remainingTurns_;
    }
}
