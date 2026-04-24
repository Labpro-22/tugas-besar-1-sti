#ifndef RENT_PAYMENT_FAILED_HPP
#define RENT_PAYMENT_FAILED_HPP

#include "InsufficientFundsException.hpp"
#include <string>

class RentPaymentFailed : public InsufficientFundsException {
public:
    explicit RentPaymentFailed(int amount)
        : InsufficientFundsException(
              214,
              "Pembayaran sewa gagal. Dana tidak mencukupi untuk membayar sewa: M" + std::to_string(amount)) {}

    virtual ~RentPaymentFailed() noexcept = default;
};

#endif