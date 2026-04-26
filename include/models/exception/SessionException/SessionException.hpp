#ifndef SESSION_EXCEPTION_HPP
#define SESSION_EXCEPTION_HPP

#include "../GeneralException.hpp"
#include <string>

class SessionException : public GeneralException {
public:
    SessionException(int code, const std::string& message)
        : GeneralException(message, code) {}

    virtual ~SessionException() noexcept = default;
};

#endif