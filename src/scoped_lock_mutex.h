#pragma once
#include <mutex>

// it appears there's no scoped_lock yet for std::mutex, so I'm making my own

class CScopedLockMutex
{
public:
    CScopedLockMutex(std::mutex& mutex)
    : m_rMutex(mutex)
    {
        mutex.lock();
    }

    virtual ~CScopedLockMutex()
    {
        m_rMutex.unlock();
    }

    std::mutex& m_rMutex;
};
