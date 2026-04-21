#include "LoadConfigFailed.hpp"

LoadConfigFailed::LoadConfigFailed(const std::string& fileName)
    : ConfigException(1001, "Failed to load config file: " + fileName) {}