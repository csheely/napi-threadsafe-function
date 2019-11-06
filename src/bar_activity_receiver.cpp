#include "bar_activity_receiver.h"

CBarActivityReceiver::CBarActivityReceiver(const std::string& sName, Napi::Env& env, Napi::Function& callback)
: m_sName(sName)
{
    m_Callback = Napi::ThreadSafeFunction::New(env, callback, sName.c_str(), 0, 1);
}

CBarActivityReceiver::~CBarActivityReceiver()
{
    m_Callback.Release();
}

void CBarActivityReceiver::SendCallback(uint32_t* pCallbackData)
{
    // the BlockingCall() method takes a lambda function that gets
    // executed in the context of the main Node.js execution thread.
    // Then it's this lambda function that generates the arguments
    // and explicitly calls the "real" javascript callback function
    m_Callback.BlockingCall(pCallbackData, [](Napi::Env env, Napi::Function jsCallback, uint32_t* pCallbackData)
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

