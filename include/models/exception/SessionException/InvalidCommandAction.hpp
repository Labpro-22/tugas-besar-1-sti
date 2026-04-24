#ifndef INVALID_COMMAND_ACTION_HPP
#define INVALID_COMMAND_ACTION_HPP

#include "SessionException.hpp"
#include <string>

class InvalidCommandAction : public SessionException {
public:
    explicit InvalidCommandAction(const std::string& action)
        : SessionException(302, "Perintah tidak valid: " + action) {}

    virtual ~InvalidCommandAction() noexcept = default;
};

#endif