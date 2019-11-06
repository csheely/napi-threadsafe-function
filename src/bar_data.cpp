#include "bar_data.h"

CBarData::CBarData(const std::string& sName, const uint32_t& nId, Napi::Env& env, Napi::Function& callback)
: m_sName(sName)
, m_nId(nId)
, m_bPendingDeletion(false)
{
    m_Callback = Napi::ThreadSafeFunction::New(env, callback, sName.c_str(), 0, 1);
}

CBarData::~CBarData()
{
    m_Callback.Release();
}

bool CBarData::Register()
{
    bool bResult = IsValid();

    return bResult;
}

bool CBarData::Unregister()
{
    bool bResult = IsValid();

    return bResult;
}

void CBarData::ClearData()
{
}

