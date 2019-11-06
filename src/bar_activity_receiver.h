#pragma once
#include <string>
#include <cstdint>
#include "napi.h"

class CBarActivityReceiver
{
public:
    CBarActivityReceiver(const std::string& sName, Napi::Env& env, Napi::Function& callback);
    virtual ~CBarActivityReceiver();

    std::string GetName() { return m_sName; }

protected:
    void SendCallback(uint32_t* pData);

    Napi::ThreadSafeFunction m_Callback;

    std::string m_sName;
};

typedef std::shared_ptr<CBarActivityReceiver>  CBarActivityReceiverPtr;
