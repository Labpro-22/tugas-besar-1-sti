#ifndef GENERAL_EXCEPTION_HPP
#define GENERAL_EXCEPTION_HPP

#include <exception>
#include <string>

class GeneralException : public std::exception {
protected:
    std::string errorMsg_;
    int errorCode_;

public:
    GeneralException(const std::string& message, int code)
        : errorMsg_(message), errorCode_(code) {}

    virtual ~GeneralException() noexcept = default;

    virtual const char* what() const noexcept override {
        return errorMsg_.c_str();
    }

    std::string getErrorMessage() const {
        return errorMsg_;
    }

    int getErrorCode() const {
        return errorCode_;
    }
};

#endif