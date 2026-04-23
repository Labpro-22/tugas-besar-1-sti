#ifndef NEW_CONFIG_FAILED_HPP
#define NEW_CONFIG_FAILED_HPP

#include "ConfigException.hpp"
#include <string>

class NewConfigFailed : public ConfigException {
public:
    explicit NewConfigFailed(const std::string& fileName)
        : ConfigException(102, "Gagal membuat file konfigurasi baru: " + fileName) {}

    virtual ~NewConfigFailed() noexcept = default;
};

#endif