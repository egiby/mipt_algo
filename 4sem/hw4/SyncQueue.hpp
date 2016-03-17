#include "Futex.hpp"

#include <queue>
#include <mutex>

template<typename T>
class ISyncQueue
{
public:
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual bool empty() = 0;
    virtual size_t size() = 0;
    virtual void push(const T&) = 0;
    virtual void pop() = 0;
    virtual const T& front() = 0;
    virtual const T& back() = 0;
    
    virtual ~ISyncQueue()
    {
    }
};


template<typename T, class Mutex = Futex>
class SyncQueue: public ISyncQueue<T>
{
    Mutex locker;
    Mutex local_locker;
    std::queue<T> queue;
public:
    
    void lock()
    {
        locker.lock();
    }
    
    void unlock()
    {
        locker.unlock();
    }
    
    bool empty()
    {
        std::lock_guard<Mutex> l(local_locker);
        return queue.empty();
    }
    
    size_t size()
    {
        std::lock_guard<Mutex> l(local_locker);
        return queue.size();
    }
    
    void push(const T &element)
    {
        std::lock_guard<Mutex> l(local_locker);
        queue.push(element);
    }
    
    void pop()
    {
        std::lock_guard<Mutex> l(local_locker);
        queue.pop();
    }
    
    const T& front()
    {
        std::lock_guard<Mutex> l(local_locker);
        return queue.front();
    }
    
    const T& back()
    {
        std::lock_guard<Mutex> l(local_locker);
        return queue.back();
    }
};
