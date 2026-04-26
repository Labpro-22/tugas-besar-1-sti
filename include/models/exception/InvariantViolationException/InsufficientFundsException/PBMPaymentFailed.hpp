#ifndef PBM_PAYMENT_FAILED_HPP
#define PBM_PAYMENT_FAILED_HPP

#include "InsufficientFundsException.hpp"
#include <string>

class PBMPaymentFailed : public InsufficientFundsException {
public:
    explicit PBMPaymentFailed(int amount)
        : InsufficientFundsException(
              211,
              "Pembayaran PBM gagal. Dana tidak mencukupi untuk membayar PBM: M" + std::to_string(amount)) {}

    virtual ~PBMPaymentFailed() noexcept = default;
};

#endif