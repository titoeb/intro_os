#include <thread>
#include <iostream>
#include <vector>

using namespace std;

const int NUM_THREADS = 16;

void say_hello(int my_thread_id)
{
    cout << "Hello, I am thread" << my_thread_id << endl;
    my_thread_id++;
}

int main(void)
{
    vector<thread> thread_pool;

    // Create all the threads and let them run.
    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_pool.push_back(thread(say_hello, i));
    }

    // Now collect all the threads and join them.
    for (auto &th : thread_pool)
    {
        if (th.joinable())
            th.join();
    }
    return 0;
}