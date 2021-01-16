#include <thread>
#include <iostream>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>

using namespace std;

void producer(deque<int> &buffer,
              int producer_num,
              mutex &m,
              condition_variable &c_consumer,
              condition_variable &c_producers,
              const int maximum_buffer_size,
              int start_val,
              int end_val)
{
    for (int i = start_val; i < end_val; i++)
    {
        // Lock Mutex for this scope.
        unique_lock<mutex> lock(m);
        if (buffer.size() > maximum_buffer_size)
        {
            throw("Buffer is too long!");
        }

        while (buffer.size() == maximum_buffer_size)
        {
            c_producers.wait_for(lock, 100ms);
        }
        buffer.push_back(i);

        // Unlock mutex
        lock.unlock();

        // Signal a single Consumer
        c_consumer.notify_one();

        // logging
        cout << "Producer " << producer_num << " produced " << i << "." << endl
             << flush;
    }
}

void consumer(deque<int> &buffer,
              int consumer_num,
              mutex &m,
              condition_variable &c_consumer,
              condition_variable &c_producer,
              int max_val)
{
    int elem = -1, n_rounds = 0;
    while (n_rounds < max_val)
    {
        unique_lock<mutex> lock(m);
        while (buffer.size() == 0)
        {
            c_consumer.wait_for(lock, 100ms);
        }

        // Remove element from buffer
        elem = buffer.back();
        buffer.pop_back();
        n_rounds++;

        lock.unlock();
        c_producer.notify_one();

        cout << "Consumer " << consumer_num << " consumed value " << elem << "." << endl
             << flush;
    }
}

int main(void)
{
    // Parameterization
    const int max_buffer_length = 4, max_val = 100, n_producers = 4, n_consumers = 1;

    // Set-up of condition variables and mutex.
    mutex m;
    condition_variable c_consumer, c_producer;

    // Data structures for
    vector<thread> thread_pool;
    deque<int> buffer;

    // Create all producers.
    for (int i = 0; i < n_producers; i++)
    {
        int start_val = i * (max_val / n_producers);
        int end_val = (i + 1) * (max_val / n_producers);

        thread_pool.push_back(thread(producer, ref(buffer), i, ref(m), ref(c_consumer), ref(c_producer), max_buffer_length, start_val, end_val));
    }

    // Create all consumers
    for (int i = 0; i < n_consumers; i++)
    {
        thread_pool.push_back(thread(consumer, ref(buffer), i, ref(m), ref(c_consumer), ref(c_producer), max_val));
    }

    // Collect all the threads (consumers + producers) and join them.
    for (auto &th : thread_pool)
    {
        if (th.joinable())
            th.join();
    }
    return 0;
}