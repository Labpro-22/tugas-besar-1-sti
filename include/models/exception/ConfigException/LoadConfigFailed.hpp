#ifndef LOAD_CONFIG_FAILED_HPP
#define LOAD_CONFIG_FAILED_HPP

#include "ConfigException.hpp"
#include <string>

class LoadConfigFailed : public ConfigException {
public:
    explicit LoadConfigFailed(const std::string& fileName)
        : ConfigException(101, "Gagal memuat file konfigurasi: " + fileName) {}

    virtual ~LoadConfigFailed() noexcept = default;
};

#endif