#ifndef SAVE_CONFIG_FAILED_HPP
#define SAVE_CONFIG_FAILED_HPP

#include "ConfigException.hpp"
#include <string>

class SaveConfigFailed : public ConfigException {
public:
    explicit SaveConfigFailed(const std::string& fileName)
        : ConfigException(103, "Gagal menyimpan file konfigurasi: " + fileName) {}

    virtual ~SaveConfigFailed() noexcept = default;
};

#endif