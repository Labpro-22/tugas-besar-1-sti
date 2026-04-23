#ifndef CONFIG_EXCEPTION_HPP
#define CONFIG_EXCEPTION_HPP

#include "../GeneralException.hpp"
#include <string>

class ConfigException : public GeneralException {
public:
    ConfigException(int code, const std::string& message)
        : GeneralException(message, code) {}

    virtual ~ConfigException() noexcept = default;
};

#endif