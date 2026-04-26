#ifndef PROPERTY_PAYMENT_FAILED_HPP
#define PROPERTY_PAYMENT_FAILED_HPP

#include "InsufficientFundsException.hpp"
#include <string>

class PropertyPaymentFailed : public InsufficientFundsException {
public:
    explicit PropertyPaymentFailed(int amount)
        : InsufficientFundsException(
              213,
              "Pembayaran properti gagal. Dana tidak mencukupi untuk membeli properti bernilai: M" + std::to_string(amount)) {}

    virtual ~PropertyPaymentFailed() noexcept = default;
};

#endif