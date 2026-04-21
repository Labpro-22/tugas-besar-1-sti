#ifndef GENERAL_EXCEPTION_HPP
#define GENERAL_EXCEPTION_HPP

#include <exception>
#include <string>

class GeneralException : public std::exception
{
protected:
    std::string errorMsg_;
    int errorCode_;

public:
    GeneralException(const std::string& message, int code);
    virtual ~GeneralException() noexcept;

    virtual const char* what() const noexcept override;

    std::string getErrorMessage() const;
    int getErrorCode() const;
};

#endif