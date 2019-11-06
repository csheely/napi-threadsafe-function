#pragma once
#include <napi.h>
#include "bar_activity_receiver.h"

class CBarInterface : public Napi::ObjectWrap<CBarInterface>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env);
    CBarInterface(const Napi::CallbackInfo& info);

    void SetData(CBarActivityReceiverPtr pData);
    CBarActivityReceiverPtr GetData();
    void ClearData();

    bool Register();
    bool Unregister();

private:
    Napi::Value Bar1Request(const Napi::CallbackInfo& info);
    Napi::Value Bar2Request(const Napi::CallbackInfo& info);

    static Napi::FunctionReference constructor;

    CBarActivityReceiverPtr    m_pBarActivityReceiver;
};
