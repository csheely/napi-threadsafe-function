#pragma once

#include <napi.h>
#include "bar_handler.h"

class CFooInterface: public Napi::ObjectWrap<CFooInterface>
{
public:
    CFooInterface(const Napi::CallbackInfo& info);
    ~CFooInterface();

    static Napi::Object Init(Napi::Env env, Napi::Object exports);

private:
    static Napi::FunctionReference constructor;
    
    Napi::Value Cleanup(const Napi::CallbackInfo& info);
    Napi::Value RegisterBar(const Napi::CallbackInfo& info);
    Napi::Value DeregisterBar(const Napi::CallbackInfo& info);

    bool CleanupHelper();

    CBarHandler      m_BarHandler;
    std::string      m_sName;
};
