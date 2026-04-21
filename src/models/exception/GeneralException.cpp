#include "GeneralException.hpp"

GeneralException::GeneralException(const std::string& message, int code)
    : errorMsg_(message), errorCode_(code) {}

GeneralException::~GeneralException() noexcept {}

const char* GeneralException::what() const noexcept
{
    return errorMsg_.c_str();
}

std::string GeneralException::getErrorMessage() const
{
    return errorMsg_;
}

int GeneralException::getErrorCode() const
{
    return errorCode_;
}