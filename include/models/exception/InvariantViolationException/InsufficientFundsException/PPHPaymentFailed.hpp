#ifndef PPH_PAYMENT_FAILED_HPP
#define PPH_PAYMENT_FAILED_HPP

#include "InsufficientFundsException.hpp"
#include <string>

class PPHPaymentFailed : public InsufficientFundsException {
public:
    explicit PPHPaymentFailed(int amount)
        : InsufficientFundsException(
              212,
              "Pembayaran PPH gagal. Dana tidak mencukupi untuk membayar PPH: M" + std::to_string(amount)) {}

    virtual ~PPHPaymentFailed() noexcept = default;
};

#endif