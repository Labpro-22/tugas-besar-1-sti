#ifndef LOAD_CONFIG_FAILED_HPP
#define LOAD_CONFIG_FAILED_HPP

#include "ConfigException.hpp"

class LoadConfigFailed : public ConfigException
{
public:
    LoadConfigFailed(const std::string& fileName);
};

#endif