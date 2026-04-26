#ifndef CARD_PAYMENT_FAILED_EXCEPTION_HPP
#define CARD_PAYMENT_FAILED_EXCEPTION_HPP

#include "../InvariantViolationException.hpp"
#include <string>

class CardPaymentFailedException : public InvariantViolationException {
public:
    CardPaymentFailedException(int code, const std::string& message)
        : InvariantViolationException(code, message) {}

    virtual ~CardPaymentFailedException() noexcept = default;
};

#endif