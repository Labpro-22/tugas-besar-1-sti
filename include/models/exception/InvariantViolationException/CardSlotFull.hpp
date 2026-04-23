#ifndef CARD_SLOT_FULL_HPP
#define CARD_SLOT_FULL_HPP

#include "InvariantViolationException.hpp"
#include <string>

class CardSlotFull : public InvariantViolationException {
public:
    CardSlotFull()
        : InvariantViolationException(201, "Slot kartu penuh. Kapasitas maksimum kartu tercapai.") {}

    virtual ~CardSlotFull() noexcept = default;
};

#endif