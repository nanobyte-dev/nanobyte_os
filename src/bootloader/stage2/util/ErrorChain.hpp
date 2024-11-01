#pragma once
#include <util/ResultCode.hpp>
#include <etl/vector.h>
#include <etl/string.h>

#define ERROR_CHAIN_MAX_DEPTH 10
#define ERROR_CHAIN_MAX_BUFFER 256

#define Fail(code, message) Fail_(__FILE__, __LINE__, code, message)
#define FailOnError(code, message) FailOnError_(__FILE__, __LINE__, code, message)

class ErrorChain
{
public:
    ErrorChain();
    void Fail_(const char* file, int line, ResultCode code, const char* message);
    void FailOnError_(const char* file, int line, ResultCode code, const char* message);
    void Clear();

    bool Failed() { return m_Failed; }
    ResultCode LastError() { return m_Errors.back(); }
    etl::string_view Message() { return m_Message; }
    const etl::ivector<ResultCode>& Errors() const { return m_Errors; }

private:
    void AppendMessage(const char* file, int line, ResultCode code, const char* message);

    bool m_Failed;
    etl::vector<ResultCode, ERROR_CHAIN_MAX_DEPTH> m_Errors;
    etl::string<ERROR_CHAIN_MAX_BUFFER> m_Message;
};