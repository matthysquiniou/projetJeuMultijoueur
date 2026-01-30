#pragma once
class ThreadWrapper {
public:
    ThreadWrapper(); 
    ~ThreadWrapper();

    void Start(LPTHREAD_START_ROUTINE func, void* param);
    void Stop();
    bool IsRunning() const;

private:
    HANDLE m_handle;
    bool m_running;
};
