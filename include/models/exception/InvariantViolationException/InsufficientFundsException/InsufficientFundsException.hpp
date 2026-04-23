#ifndef INSUFFICIENT_FUNDS_EXCEPTION_HPP
#define INSUFFICIENT_FUNDS_EXCEPTION_HPP

#include "../InvariantViolationException.hpp"
#include <string>

class InsufficientFundsException : public InvariantViolationException {
public:
    InsufficientFundsException(int code, const std::string& message)
        : InvariantViolationException(code, message) {}

    virtual ~InsufficientFundsException() noexcept = default;
};

#endif