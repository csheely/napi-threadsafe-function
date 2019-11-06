#pragma once
#include <cstdint>
#include <string>
#include "napi.h"
#include "bar_data.h"

class CBarCallbackData
{
public:
    CBarCallbackData(const char* const sEventName, CBarDataPtr pData, bool bIsResponse)
    : m_sEventName(sEventName)
    , m_sBarName(pData->m_sName)
    , m_nId(pData->m_nId)
    , m_bIsResponse(bIsResponse)
    { }

    virtual ~CBarCallbackData()
    {
    }

    std::string const EventName()
    {
        return m_sEventName;
    }

    void JavascriptInator(Napi::Env& env, Napi::Object& obj)
    {
        obj.Set("name", Napi::String::New(env, m_sBarName.c_str()));
        obj.Set("id", Napi::Number::New(env, m_nId));
        obj.Set("isResponse", Napi::Boolean::New(env, m_bIsResponse));
    }

protected:
    std::string m_sEventName;
    std::string m_sBarName;
    uint32_t    m_nId;
    bool        m_bIsResponse;
};

