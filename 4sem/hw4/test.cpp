#include "SyncQueue.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <cassert>

using std::cerr;

typedef unsigned int ui32;

std::default_random_engine generator(501);

class SyncQueueTest: public ::testing::TestWithParam<ui32>
{
};

void getPush(SyncQueue<ui32> &queue, ui32 x)
{
    std::lock_guard<SyncQueue<ui32> > l(queue);
    queue.push(x);
}

void getPop(SyncQueue<ui32> &queue)
{
    std::lock_guard<SyncQueue<ui32> > l(queue);
    queue.pop();
}

TEST_P(SyncQueueTest, TestOfPush)
{
    SyncQueue<ui32> queue;
    std::thread threads[10];
    for (ui32 i = 0; i < GetParam(); ++i)
    {
        threads[i % 10] = std::thread(getPush, std::ref(queue), i);
        if (i)
            threads[(i - 1) % 10].join();
    }
    
    threads[(GetParam() - 1) % 10].join();
    
    std::vector<ui32> used(GetParam());
    
    for (ui32 i = 0; i < GetParam(); ++i)
    {
        used[queue.front()] = 1;
        queue.pop();
    }
    
    ASSERT_EQ(std::accumulate(used.begin(), used.end(), 0), GetParam());
}

INSTANTIATE_TEST_CASE_P(TestName, SyncQueueTest, ::testing::Values(100, 200));

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
