#include "bar_handler.h"
#include "scoped_lock_mutex.h"
#include "bar_callback_data.h"
#include "bar_interface.h"

CBarHandler::CBarHandler()
{
}

CBarHandler::~CBarHandler()
{
    Cleanup();
}

/// <summary>Force-cleanup of any/all endpoint data</summary>
/// 
void CBarHandler::Cleanup()
{
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
}

bool CBarHandler::CreateBarInterface(const std::string& sName, Napi::Env& env, Napi::Function& callback, Napi::Object& BarIf)
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

void CBarHandler::CleanupBarInterface(Napi::Object& BarIf)
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

void CBarHandler::SendCallback(CBarDataPtr pBarData, CBarCallbackData* pCallbackData)
{
#if 0
    // the BlockingCall() method takes a lambda function that gets
    // executed in the context of the main Node.js execution thread.
    // Then it's this lambda function that generates the arguments
    // and explicitly calls the "real" javascript callback function
    pBarData->m_Callback.BlockingCall(pCallbackData, [](Napi::Env env, Napi::Function jsCallback, CBarCallbackData* pCallbackData)
    {
        Napi::Object Arguments = Napi::Object::New(env);

        pCallbackData->JavascriptInator(env, Arguments);

        jsCallback.Call( { 
            Napi::String::New(env, pCallbackData->EventName()),
            Arguments
        } );

        delete pCallbackData;
    });
#endif
}
