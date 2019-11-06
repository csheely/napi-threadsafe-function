#pragma once
#include <mutex>
#include <map>
#include <cstdint>
#include "bar_data.h"

class CBarCallbackData;

class CBarHandler
{
public:
    CBarHandler();
    virtual ~CBarHandler();

    void Cleanup();
    
    bool CreateBarInterface(const std::string& sName, Napi::Env& env, Napi::Function& callback, Napi::Object& pBarIf);
    void CleanupBarInterface(Napi::Object& pBarIf);

protected:
    void SendCallback(CBarDataPtr pEndpointData, CBarCallbackData* pData);

    std::mutex  m_Mutex;
    std::map<uint32_t, CBarDataPtr> m_IdBarMap;
    std::map<std::string, CBarDataPtr> m_NameBarMap;

};
