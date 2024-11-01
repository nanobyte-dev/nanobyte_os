#include "ErrorChain.hpp"
#include <etl/to_string.h>

ErrorChain::ErrorChain()
    : m_Errors(),
      m_Failed(false),
      m_Message()
{
}

void ErrorChain::Fail_(const char *file, int line, ResultCode code, const char *message)
{
    m_Failed = true;
    m_Errors.push_back(code);
    AppendMessage(file, line, code, message);
}

void ErrorChain::FailOnError_(const char *file, int line, ResultCode code, const char *message)
{
    if (m_Failed)
    {
        m_Failed = true;
        m_Errors.push_back(code);
        AppendMessage(file, line, code, message);
    }
}

void ErrorChain::Clear()
{
    m_Failed = false;
    m_Errors.clear();
    m_Message.clear();
}

void ErrorChain::AppendMessage(const char *file, int line, ResultCode code, const char *message)
{
    m_Message.append(file);
    m_Message.append(":");
    etl::to_string(line, m_Message, true);
    m_Message.append(": [");
    etl::to_string((int)code, m_Message, true);
    m_Message.append("] ");
    m_Message.append(message);
    m_Message.append("\n");
}
