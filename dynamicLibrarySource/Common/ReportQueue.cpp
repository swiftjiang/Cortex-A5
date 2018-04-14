////////////////////////////////////////////////////////////////////////////////
// ReportQueue.cpp
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "ReportQueue.h"

////////////////////////////////////////////////////////////////////////////////
// CReportQueue Class - Constructor/Destructor
////////////////////////////////////////////////////////////////////////////////

CReportQueue::CReportQueue()
{
    pthread_mutexattr_t attr;

    // Create a mutex object with recursive behavior
    //
    // This means that you can call pthread_mutex_lock()
    // more than once from the same thread before calling
    // pthread_mutex_unlock().
    //
    // Doing the same using the default mutex attributes
    // would cause deadlock.
    //
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_cs, &attr);
    pthread_mutexattr_destroy(&attr);
}

CReportQueue::~CReportQueue()
{
    pthread_mutex_destroy(&m_cs);
}

////////////////////////////////////////////////////////////////////////////////
// CReportQueue Class - Public Methods
////////////////////////////////////////////////////////////////////////////////

void CReportQueue::Clear()
{
    pthread_mutex_lock(&m_cs);

    m_queue.clear();

    pthread_mutex_unlock(&m_cs);
}

void CReportQueue::Reserve(DWORD count)
{
    pthread_mutex_lock(&m_cs);

    m_queue.reserve(count);

    pthread_mutex_unlock(&m_cs);
}

void CReportQueue::Enqueue(const BYTE* data, DWORD size)
{
    pthread_mutex_lock(&m_cs);

    for (DWORD i = 0; i < size; i++)
    {
        m_queue.push_back(data[i]);
    }

    pthread_mutex_unlock(&m_cs);
}

void CReportQueue::Dequeue(BYTE* data, DWORD size, DWORD* bytesDequeued)
{
    pthread_mutex_lock(&m_cs);

    // Figure out how many bytes to dequeue
    DWORD num = (size < m_queue.size()) ? size : m_queue.size();

    // Copy the bytes to the user buffer
    for (DWORD i = 0; i < num; i++)
    {
        data[i] = m_queue[i];
    }

    // Remove the bytes from the queue
    m_queue.erase(m_queue.begin(), m_queue.begin() + num);

    // Return the number of bytes dequeued
    *bytesDequeued = num;

    pthread_mutex_unlock(&m_cs);
}

DWORD CReportQueue::Size()
{
    pthread_mutex_lock(&m_cs);

    DWORD size = (DWORD) m_queue.size();

    pthread_mutex_unlock(&m_cs);

    return size;
}
