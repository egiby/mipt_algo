#include "Futex.hpp"

#include <iostream>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <vector>
#include <cassert>

template<class MutexType>
void inc(long long * local_sum, long long * sum, const long long SUM, MutexType * m)
{
    //~ std::cerr << "I'm doing smth\n";
    for (int i = 0; i < SUM; ++i)
    {
        std::lock_guard<MutexType> lg(*m);
        (*sum)++;
        (*local_sum)++;
    }
}

template<class MutexType>
void printSums(int number_threads, const long long SUM)
{
    auto start_time = std::chrono::system_clock::now();
    std::vector<std::thread> t(number_threads);
    std::vector<long long> local_sum(number_threads);
    MutexType m;
    long long sum(0);
    
    for (int i = 0; i < number_threads; ++i)
        t[i] = std::thread(inc<MutexType>, &local_sum[i], &sum, SUM / number_threads, &m);
    
    for (int i = 0; i < number_threads; ++i)
        t[i].join();
    
    auto end_time = std::chrono::system_clock::now();
    
    assert(sum == SUM);
    
    std::cout << "result sum: " << std::accumulate(local_sum.begin(), local_sum.end(), 0ll) << '\n';
    for (auto s: local_sum)
        std::cout << s << '\n';
    std::cout << '\n';
    
    std::cout << "answer: " << sum << '\n';
    std::cout << std::chrono::duration<double>(end_time - start_time).count() << "s\n";
}

int main()
{
    std::cout << "Task B. Futex 10 seconds\n";
    printSums<Futex>(10, 350000000);
    
    std::cout << "\n\nTask C. Futex 60 seconds\n";
    printSums<Futex>(10, 2500000000);
    
    std::cout << "\n\nTask D.\n";
   
    std::cout << "std::mutex with std::thread::hardware_concurrency() / 2 treads\n";
    printSums<std::mutex>(std::thread::hardware_concurrency() / 2, 350000000);
    std::cout << "\n\nstd::mutex with std::thread::hardware_concurrency() * 2 treads\n";
    printSums<std::mutex>(std::thread::hardware_concurrency() * 2, 350000000);
    
    std::cout << "\n\nFutex with std::thread::hardware_concurrency() / 2 treads\n";
    printSums<Futex>(std::thread::hardware_concurrency() / 2, 350000000);
    std::cout << "\n\nFutex with std::thread::hardware_concurrency() * 2 treads\n";
    printSums<Futex>(std::thread::hardware_concurrency() * 2, 350000000);
    return 0;
}
