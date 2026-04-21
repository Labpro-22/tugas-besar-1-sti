#include "ConfigException.hpp"

ConfigException::ConfigException(int code, const std::string& message)
    : GeneralException(message, code) {}