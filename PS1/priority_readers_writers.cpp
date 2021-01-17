#include <thread>
#include <iostream>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <time.h>
#include <chrono>

using namespace std;

void writer(int &shared_var,
            int writer_id,
            mutex &m,
            condition_variable &c_readers,
            condition_variable &c_writers,
            int n_writes,
            int &n_readers)
{
    for (int write = 0; write < n_writes; write++)
    {
        // Wait for random time.
        this_thread::sleep_for(chrono::milliseconds(rand() % 10 + 1));

        // Lock Mutex for this scope.
        unique_lock<mutex> lock(m);
        while (n_readers != 0)
        {
            c_writers.wait_for(lock, 100ms);
        }
        // Another reader reads now
        n_readers = -1;
        lock.unlock();

        // Write random number
        shared_var = rand() % 1000;
        cout << "Writer " << writer_id << " wrote " << shared_var << " and there are "
             << n_readers << " other readers." << endl
             << flush;

        // Signal reader to read.
        lock.lock();
        n_readers = 0;
        c_readers.notify_all();
        lock.unlock();
    }
}

void reader(int &shared_var,
            int reader_id,
            mutex &m,
            condition_variable &c_readers,
            condition_variable &c_writers,
            int n_reads,
            int &n_readers)
{
    for (int read = 0; read < n_reads; read++)
    { // Wait for random time.
        this_thread::sleep_for(chrono::milliseconds(rand() % 10 + 1));

        // Lock Mutex for this scope.
        unique_lock<mutex> lock(m);
        while (n_readers == -1)
        {
            c_readers.wait_for(lock, 100ms);
        }
        // Another reader reads now
        n_readers++;
        int n_other_readers = n_readers - 1;
        lock.unlock();

        // Read the current number
        cout << "Reader " << reader_id << " read " << shared_var
             << " and there are curently " << n_other_readers
             << " other reader." << endl
             << flush;

        // Signal reader to read.
        lock.lock();
        n_readers--;
        if (n_readers == 0)
        {
            c_writers.notify_one();
        }
        lock.unlock();
    }
}

int main(void)
{
    // Parameterization
    const int n_reader = 5, n_writer = 5, n_reads = 3, n_writes = 3;

    // Set-up of condition variables and mutex.
    mutex m;
    condition_variable c_readers, c_writers;

    // Data structures for threads
    vector<thread> thread_pool;

    // Shared Variable
    int shared_variable = -1;
    int n_readers = 0;

    srand(time(NULL));

    // Create all writers.
    for (int i = 0; i < n_writer; i++)
    {

        thread_pool.push_back(thread(writer, ref(shared_variable), i, ref(m), ref(c_readers), ref(c_writers), n_writes, ref(n_readers)));
    }

    // Create all readers
    for (int i = 0; i < n_reader; i++)
    {
        thread_pool.push_back(thread(reader, ref(shared_variable), i, ref(m), ref(c_readers), ref(c_writers), n_reads, ref(n_readers)));
    }

    // Collect all the threads (consumers + producers) and join them.
    for (auto &th : thread_pool)
    {
        if (th.joinable())
            th.join();
    }
    return 0;
}