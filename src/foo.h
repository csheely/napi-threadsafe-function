#pragma once

#include <napi.h>
#include <map>
#include <cstdint>
#include "bar_data.h"

class CFoo: public Napi::ObjectWrap<CFoo>
{
public:
    CFoo(const Napi::CallbackInfo& info);
    ~CFoo();

    static Napi::Object Init(Napi::Env env, Napi::Object exports);


private:
    static Napi::FunctionReference constructor;
    
    Napi::Value Cleanup(const Napi::CallbackInfo& info);
    Napi::Value RegisterBar(const Napi::CallbackInfo& info);
    Napi::Value DeregisterBar(const Napi::CallbackInfo& info);

    bool CleanupHelper();

    bool CreateBarInterface(const std::string& sName, Napi::Env& env, Napi::Function& callback, Napi::Object& pBarIf);
    void CleanupBarInterface(Napi::Object& pBarIf);

    void SendCallback(CBarDataPtr pEndpointData, uint32_t* pData);

    std::map<uint32_t, CBarDataPtr> m_IdBarMap;
    std::map<std::string, CBarDataPtr> m_NameBarMap;

    std::string      m_sName;
};
