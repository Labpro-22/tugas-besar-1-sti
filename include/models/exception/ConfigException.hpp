#ifndef CONFIG_EXCEPTION_HPP
#define CONFIG_EXCEPTION_HPP

#include "GeneralException.hpp"

class ConfigException : public GeneralException
{
public:
    ConfigException(int code, const std::string& message);
};

#endif