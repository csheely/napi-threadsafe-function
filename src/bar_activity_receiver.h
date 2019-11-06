#pragma once
#include <string>
#include <cstdint>
#include "napi.h"

class CBarActivityReceiver
{
public:
    CBarActivityReceiver(const std::string& sName, const uint32_t& nId, Napi::Env& env, Napi::Function& callback);
    virtual ~CBarActivityReceiver();

    bool IsValid() { return !m_bPendingDeletion; }
    void SetDeleteFlag() { m_bPendingDeletion = true; }
    bool Register();
    bool Unregister();
    void ClearData();

    std::string m_sName;
    uint32_t    m_nId;

protected:
    void SendCallback(uint32_t* pData);

    Napi::ThreadSafeFunction m_Callback;

    bool      m_bPendingDeletion;
};

typedef std::shared_ptr<CBarActivityReceiver>  CBarActivityReceiverPtr;
