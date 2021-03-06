#include "foo.h"
#include "bar_interface.h"

// Declaration of static objects in CFoo
Napi::FunctionReference CFoo::constructor;

// Method to define how the class is exposed in Javascript.
Napi::Object CFoo::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope   scope(env);

    Napi::Function  func = DefineClass(env, "Foo", {
        InstanceMethod("cleanup", &CFoo::Cleanup),
        InstanceMethod("registerBar", &CFoo::RegisterBar),
        InstanceMethod("deregisterBar", &CFoo::DeregisterBar)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Foo", func);

    return exports;
}

// Constructor.
CFoo::CFoo(const Napi::CallbackInfo& info)
: Napi::ObjectWrap<CFoo>(info)
{
    Napi::Env           env = info.Env();
    Napi::HandleScope   scope(env);

    // Make sure JS arguments are what we expect.
    if (info.Length() != 1)
    {
        Napi::TypeError::New(env, "Invalid number of arguments to Foo constructor").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Invalid argument types to Foo constructor").ThrowAsJavaScriptException();
        return;
    }

    m_sName = info[0].As<Napi::String>().Utf8Value();
}

// Destructor.  Cleanup any underlying references.
CFoo::~CFoo()
{
    CleanupHelper();
}

// Do whatever it takes to shut down the lower-level activity associated with this
// object.  Currently no way to reuse the object, but this method must be called
// for low-level cleanup.
Napi::Value CFoo::Cleanup(const Napi::CallbackInfo& info)
{
    Napi::Env   env = info.Env();
    Napi::HandleScope   scope(env);
    Napi::Value nullReturnVal;

    if (CleanupHelper() == false)
    {
        Napi::Error::New(env, "Cleanup() - Unspecified failure").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    return nullReturnVal;
}

// Performs the real work to cleanup the class and stop activity.
bool CFoo::CleanupHelper()
{
    bool bResult = true;

    std::map<std::string, CBarActivityReceiverPtr>::iterator it;
    CBarActivityReceiverPtr    pData;

    while (!m_NameBarMap.empty())
    {
        it = m_NameBarMap.begin();
        pData = it->second;

        m_NameBarMap.erase(it);
        pData.reset();
    }

    return bResult;
}

Napi::Value CFoo::RegisterBar(const Napi::CallbackInfo& info)
{
    Napi::Env           env = info.Env();
    Napi::HandleScope   scope(env);
    Napi::Object        BarIfObj;
    Napi::Value         nullReturnVal;

    if (info.Length() != 2)
    {
        Napi::TypeError::New(env, "CFoo::RegisterBar() - Invalid number of arguments").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    if ((!info[0].IsString()) || (!info[1].IsFunction()))
    {
        Napi::TypeError::New(env, "CFoo::RegisterBar() - Invalid argument types").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    std::string sBarName = info[0].As<Napi::String>().Utf8Value();
    Napi::Function  callback = info[1].As<Napi::Function>();

    if (CreateBarInterface(sBarName, env, callback, BarIfObj) == false)
    {
        Napi::Error::New(env, "CFoo::RegisterBar() - Bar creation error").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    CBarInterface* pBar = CBarInterface::Unwrap(BarIfObj);
    
    if (pBar->Register() == false)
    {
        CleanupBarInterface(BarIfObj);
        Napi::Error::New(env, "CFoo::RegisterBar() - Bar registration error").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    return BarIfObj;
}

Napi::Value CFoo::DeregisterBar(const Napi::CallbackInfo& info)
{
    Napi::Env           env = info.Env();
    Napi::HandleScope   scope(env);
    Napi::Value         nullReturnVal;

    if (info.Length() != 1)
    {
        Napi::TypeError::New(env, "CFoo::DeregisterBar() - Invalid number of arguments").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    if (!info[0].IsObject())
    {
        Napi::TypeError::New(env, "CFoo::DeregisterBar() - Invalid argument type").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    Napi::Object    BarIf = info[0].As<Napi::Object>();
    CBarInterface* pBar = CBarInterface::Unwrap(BarIf);

    bool bUnregResult = pBar->Unregister();

    CleanupBarInterface(BarIf);

    if (bUnregResult == false)
    {
        Napi::Error::New(env, "CFoo::DeregisterBar() - orange unregister problem").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    return nullReturnVal;
}


bool CFoo::CreateBarInterface(const std::string& sName, Napi::Env& env, Napi::Function& callback, Napi::Object& BarIf)
{
    bool                bSuccess = true;

    try
    {
        if (m_NameBarMap.find(sName) != m_NameBarMap.end())
        {
            throw std::domain_error("Bar already registered");
        }

        CBarActivityReceiverPtr pBarActivityReceiver =
            std::make_shared<CBarActivityReceiver>(sName, env, callback);

        BarIf = CBarInterface::NewInstance(env);
        CBarInterface* pIf = CBarInterface::Unwrap(BarIf);
        pIf->SetData(pBarActivityReceiver);

        m_NameBarMap[sName] = pBarActivityReceiver;
    }
    catch (...)
    {
        bSuccess = false;
        m_NameBarMap.erase(sName);

        if (!BarIf.IsEmpty())
        {
            CBarInterface* pIf = CBarInterface::Unwrap(BarIf);
            pIf->ClearData();
        }
    }

    return bSuccess;
}

void CFoo::CleanupBarInterface(Napi::Object& BarIf)
{
    if (!BarIf.IsEmpty())
    {
        CBarInterface* pBarIf = CBarInterface::Unwrap(BarIf);
        CBarActivityReceiverPtr pBarActivityReceiver = pBarIf->GetData();

        if (pBarActivityReceiver)
        {
            std::map<std::string, CBarActivityReceiverPtr>::iterator it = m_NameBarMap.find(pBarActivityReceiver->GetName());

            if (it != m_NameBarMap.end())
            {
                m_NameBarMap.erase(it);
            }

            pBarIf->ClearData();
        }
    }
}

