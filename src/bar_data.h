/* ***************************************************************************
*
*   Copyright (c) 1990-present
*
*   Avtec, LLC
*   100 Innovation Place
*   Lexington, SC 29072 USA
*   (803) 358-3600
*
*   All rights reserved. Copying, compilation, modification, distribution
*   or any other use whatsoever of this material is strictly prohibited
*   without the written consent of Avtec, LLC.
*
*************************************************************************** */
#pragma once
#include <string>
#include <cstdint>
#include "napi.h"

class CBarData
{
public:
    CBarData(const std::string& sName, const uint32_t& nId, Napi::Env& env, Napi::Function& callback);
    virtual ~CBarData();

    bool IsValid() { return !m_bPendingDeletion; }
    void SetDeleteFlag() { m_bPendingDeletion = true; }
    bool Register();
    bool Unregister();
    void ClearData();

    std::string m_sName;
    uint32_t    m_nId;

    Napi::ThreadSafeFunction m_Callback;

protected:
    bool      m_bPendingDeletion;
};

typedef std::shared_ptr<CBarData>  CBarDataPtr;
