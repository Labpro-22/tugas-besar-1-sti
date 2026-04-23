#ifndef INVARIANT_VIOLATION_EXCEPTION_HPP
#define INVARIANT_VIOLATION_EXCEPTION_HPP

#include "../GeneralException.hpp"
#include <string>

class InvariantViolationException : public GeneralException {
public:
    InvariantViolationException(int code, const std::string& message)
        : GeneralException(message, code) {}

    virtual ~InvariantViolationException() noexcept = default;
};

#endif