#include "pch.h"
#include "ThreadWrapper.h"

ThreadWrapper::ThreadWrapper() : m_handle(nullptr), m_running(false) {}

ThreadWrapper::~ThreadWrapper()
{
	Stop();
}

void ThreadWrapper::Start(LPTHREAD_START_ROUTINE func, void* param)
{
    m_running = true;
    m_handle = CreateThread(NULL, 0, func, param, 0, NULL);
}

void ThreadWrapper::Stop()
{
    if (m_handle) {
        m_running = false;
        WaitForSingleObject(m_handle, INFINITE);
        CloseHandle(m_handle);
        m_handle = nullptr;
    }
}

bool ThreadWrapper::IsRunning() const
{
    return m_running;
}
