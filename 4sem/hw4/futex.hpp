#include <atomic>
#include <exception>
#include <thread>

struct IncorrectUnlockException: public std::exception
{
    const char * what() const throw()
    {
        return "futex already unlocked or locked not in this thread";
    }
};

class Futex
{
    std::atomic<std::thread::id> locker_id;
    
public:

    explicit Futex ()
    : locker_id(static_cast<std::thread::id>(0))
    {
    }
    
    void lock()
    {
        auto id = std::this_thread::get_id();
        auto expected = static_cast<std::thread::id>(0);
        
        while (!locker_id.compare_exchange_strong(expected, id))
        {
            std::this_thread::yield();
            expected = static_cast<std::thread::id>(0);
            std::this_thread::yield();
        }
    }
    
    void unlock()
    {
        auto id = std::this_thread::get_id();
        if (!locker_id.compare_exchange_strong(id, static_cast<std::thread::id>(0)))
            throw IncorrectUnlockException();
    }
};
