#include "bar_interface.h"

Napi::FunctionReference CBarInterface::constructor;

Napi::Object CBarInterface::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope   scope(env);

    Napi::Function func = DefineClass(env, "CBarInterface", {
        InstanceMethod("bar1Request", &CBarInterface::Bar1Request),
        InstanceMethod("bar2Request", &CBarInterface::Bar2Request)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("CBarInterface", func);

    return exports;
}

CBarInterface::CBarInterface(const Napi::CallbackInfo& info)
: Napi::ObjectWrap<CBarInterface>(info)
{
}

Napi::Object CBarInterface::NewInstance(Napi::Env env)
{
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor.New({});

    return scope.Escape(napi_value(obj)).ToObject();
}

void CBarInterface::SetData(CBarDataPtr pData)
{
    m_pBarData = pData;
}

CBarDataPtr CBarInterface::GetData()
{
    return m_pBarData;
}

/// <summary>Helper to clear out data associated with the object</summary>
/// 
void CBarInterface::ClearData()
{
    m_pBarData.reset();
}

void CBarInterface::ClearInvalidData()
{
    if (m_pBarData && (!m_pBarData->IsValid()))
    {
        m_pBarData.reset();
    }
}

bool CBarInterface::Register()
{
    bool bResult = false;
    if (m_pBarData)
    {
        bResult = m_pBarData->Register();

        if (!bResult)
        {
            // check to see if we should clear our reference to the endpointdata
            ClearInvalidData();
        }
    }

    return bResult;
}

bool CBarInterface::Unregister()
{
    bool bResult = false;
    if (m_pBarData)
    {
        bResult = m_pBarData->Unregister();

        if (!bResult)
        {
            // check to see if we should clear our reference to the endpointdata
            ClearInvalidData();
        }
    }

    return bResult;
}


Napi::Value CBarInterface::Bar1Request(const Napi::CallbackInfo& info)
{
    Napi::Env           env = info.Env();
    Napi::HandleScope   scope(env);
    Napi::Value         nullReturnVal;

    if (!m_pBarData)
    {
        Napi::Error::New(env, "CBarInterface::TransmitStateRequest() - Interface detached from endpoint data").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    if (info.Length() != 0)
    {
        Napi::TypeError::New(env, "CBarInterface::TransmitStateRequest() - Invalid number of arguments").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    return nullReturnVal;
}

Napi::Value CBarInterface::Bar2Request(const Napi::CallbackInfo& info)
{
    Napi::Env           env = info.Env();
    Napi::HandleScope   scope(env);
    Napi::Value         nullReturnVal;

    if (!m_pBarData)
    {
        Napi::Error::New(env, "CBarInterface::EndpointStateRequest() - Interface detached from endpoint data").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    if (info.Length() != 0)
    {
        Napi::TypeError::New(env, "CBarInterface::EndpointStateRequest() - Invalid number of arguments").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    return nullReturnVal;
}


