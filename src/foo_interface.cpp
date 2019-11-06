#include "foo_interface.h"
#include "bar_interface.h"
#include "scoped_lock_mutex.h"

// Declaration of static objects in CFooInterface
Napi::FunctionReference CFooInterface::constructor;

// Method to define how the class is exposed in Javascript.
Napi::Object CFooInterface::Init(Napi::Env env, Napi::Object exports)
{
    Napi::HandleScope   scope(env);

    Napi::Function  func = DefineClass(env, "FooInterface", {
        InstanceMethod("cleanup", &CFooInterface::Cleanup),
        InstanceMethod("registerBar", &CFooInterface::RegisterBar),
        InstanceMethod("deregisterBar", &CFooInterface::DeregisterBar)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("FooInterface", func);

    return exports;
}

// Constructor.
CFooInterface::CFooInterface(const Napi::CallbackInfo& info)
: Napi::ObjectWrap<CFooInterface>(info)
{
    Napi::Env           env = info.Env();
    Napi::HandleScope   scope(env);

    // Make sure JS arguments are what we expect.
    if (info.Length() != 1)
    {
        Napi::TypeError::New(env, "Invalid number of arguments to FooInterface constructor").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Invalid argument types to FooInterface constructor").ThrowAsJavaScriptException();
        return;
    }

    m_sName = info[0].As<Napi::String>().Utf8Value();
}

// Destructor.  Cleanup any underlying references.
CFooInterface::~CFooInterface()
{
    CleanupHelper();
}

// Do whatever it takes to shut down the lower-level activity associated with this
// object.  Currently no way to reuse the object, but this method must be called
// for low-level cleanup.
Napi::Value CFooInterface::Cleanup(const Napi::CallbackInfo& info)
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
bool CFooInterface::CleanupHelper()
{
    bool bResult = true;

    CScopedLockMutex    lock(m_Mutex);
    std::map<std::string, CBarDataPtr>::iterator it;
    CBarDataPtr    pData;

    while (!m_NameBarMap.empty())
    {
        it = m_NameBarMap.begin();
        pData = it->second;

        // In case there's a Javascript-instantiated object (CBarInterface)
        // that still has a reference to the endpoint data object , set a delete
        // flag in the object.  This lets the holdouts know that the data is no
        // longer valid, and the reference should be cleared.
        pData->SetDeleteFlag();
        m_NameBarMap.erase(it);
        m_IdBarMap.erase(pData->m_nId);
        pData.reset();
    }

    return bResult;
}

Napi::Value CFooInterface::RegisterBar(const Napi::CallbackInfo& info)
{
    Napi::Env           env = info.Env();
    Napi::HandleScope   scope(env);
    Napi::Object        BarIfObj;
    Napi::Value         nullReturnVal;

    if (info.Length() != 2)
    {
        Napi::TypeError::New(env, "CFooInterface::RegisterBar() - Invalid number of arguments").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    if ((!info[0].IsString()) || (!info[1].IsFunction()))
    {
        Napi::TypeError::New(env, "CFooInterface::RegisterBar() - Invalid argument types").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    std::string sBarName = info[0].As<Napi::String>().Utf8Value();
    Napi::Function  callback = info[1].As<Napi::Function>();

    if (CreateBarInterface(sBarName, env, callback, BarIfObj) == false)
    {
        Napi::Error::New(env, "CFooInterface::RegisterBar() - Bar creation error").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    CBarInterface* pBar = CBarInterface::Unwrap(BarIfObj);
    
    if (pBar->Register() == false)
    {
        CleanupBarInterface(BarIfObj);
        Napi::Error::New(env, "CFooInterface::RegisterBar() - Bar registration error").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    return BarIfObj;
}

Napi::Value CFooInterface::DeregisterBar(const Napi::CallbackInfo& info)
{
    Napi::Env           env = info.Env();
    Napi::HandleScope   scope(env);
    Napi::Value         nullReturnVal;

    if (info.Length() != 1)
    {
        Napi::TypeError::New(env, "CFooInterface::DeregisterBar() - Invalid number of arguments").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    if (!info[0].IsObject())
    {
        Napi::TypeError::New(env, "CFooInterface::DeregisterBar() - Invalid argument type").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    Napi::Object    BarIf = info[0].As<Napi::Object>();
    CBarInterface* pBar = CBarInterface::Unwrap(BarIf);

    bool bUnregResult = pBar->Unregister();

    CleanupBarInterface(BarIf);

    if (bUnregResult == false)
    {
        Napi::Error::New(env, "CFooInterface::DeregisterBar() - orange unregister problem").ThrowAsJavaScriptException();
        return nullReturnVal;
    }

    return nullReturnVal;
}


bool CFooInterface::CreateBarInterface(const std::string& sName, Napi::Env& env, Napi::Function& callback, Napi::Object& BarIf)
{
    bool                bSuccess = true;
    static uint32_t     nNewId = 0;
    bool                bAllocatedId;

    CScopedLockMutex    lock(m_Mutex);

    try
    {
        if (m_NameBarMap.find(sName) != m_NameBarMap.end())
        {
            throw std::domain_error("Bar already registered");
        }

        nNewId++;
        bAllocatedId = true;

        CBarDataPtr pBarData =
            std::make_shared<CBarData>(sName, nNewId, env, callback);

        BarIf = CBarInterface::NewInstance(env);
        CBarInterface* pIf = CBarInterface::Unwrap(BarIf);
        pIf->SetData(pBarData);

        m_IdBarMap[nNewId] = pBarData;
        m_NameBarMap[sName] = pBarData;
    }
    catch (...)
    {
        bSuccess = false;
        if (bAllocatedId)
        {
            m_IdBarMap.erase(nNewId);
            m_NameBarMap.erase(sName);

            if (!BarIf.IsEmpty())
            {
                CBarInterface* pIf = CBarInterface::Unwrap(BarIf);
                pIf->ClearData();
            }
        }
    }

    return bSuccess;
}

void CFooInterface::CleanupBarInterface(Napi::Object& BarIf)
{
    if (!BarIf.IsEmpty())
    {
        CBarInterface* pBarIf = CBarInterface::Unwrap(BarIf);
        CBarDataPtr pBarData = pBarIf->GetData();

        if (pBarData)
        {
            CScopedLockMutex    lock(m_Mutex);

            std::map<uint32_t, CBarDataPtr>::iterator it = m_IdBarMap.find(pBarData->m_nId);

            if (it != m_IdBarMap.end())
            {
                m_IdBarMap.erase(it);
                m_NameBarMap.erase(pBarData->m_sName);
            }

            pBarIf->ClearData();
        }
    }
}

void CFooInterface::SendCallback(CBarDataPtr pBarData, uint32_t* pCallbackData)
{
    // the BlockingCall() method takes a lambda function that gets
    // executed in the context of the main Node.js execution thread.
    // Then it's this lambda function that generates the arguments
    // and explicitly calls the "real" javascript callback function
    pBarData->m_Callback.BlockingCall(pCallbackData, [](Napi::Env env, Napi::Function jsCallback, uint32_t* pCallbackData)
    {
        Napi::Object Arguments = Napi::Object::New(env);

        Arguments.Set("data", Napi::Number::New(env, *pCallbackData));

        jsCallback.Call( {
            Napi::String::New(env, "dataEvent"),
            Arguments
        } );

        delete pCallbackData;
    });
}
                                                                                          
