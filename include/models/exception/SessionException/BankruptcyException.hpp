#ifndef BANKRUPTCY_EXCEPTION_HPP
#define BANKRUPTCY_EXCEPTION_HPP

#include "SessionException.hpp"
#include <string>

class BankruptcyException : public SessionException {
public:
    explicit BankruptcyException(const std::string& playerName)
        : SessionException(301, "Pemain " + playerName + " bangkrut.") {}

    virtual ~BankruptcyException() noexcept = default;
};

#endif