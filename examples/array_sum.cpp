#include <iostream>
#include <cmath>
#include "thread_pool.hpp"

long ArrSum (const int* ptr, const int size)
{
    long res = 0;
    for (int i = 0; i < size; i++)
    {
        res += ptr[i];
    }

    return res;
}

long ThreadPool_ArrSum (const int* arr, const int arr_size, qlm::ThreadPool& pool)
{
    // divide the array between threads
    constexpr int cache_line_size = std::hardware_destructive_interference_size;
    const uint32_t lines = std::ceil((float)arr_size / cache_line_size);
	const int threads = std::min(lines, pool.used_threads);
    const int thread_len = (lines / threads) * cache_line_size;
    const int thread_tail = arr_size - thread_len * threads;

    std::vector<std::future<long>> futures(threads);
    long res = 0;

    // launch the threads
	int next_idx = 0;

    for (int i = 0; i < threads - 1; i++)
    {
        futures[i] = pool.Submit(ArrSum, &arr[next_idx], thread_len);
        next_idx += thread_len;
    }

    // tail thread
    futures[threads - 1] = pool.Submit(ArrSum, &arr[next_idx], thread_len + thread_tail);

    // wait for the threads to finish
    for (size_t i = 0; i < threads; i++)
    {
        res += futures[i].get();
    }

    return res;
}

int main()
{
    constexpr int arr_size = 100000000;
    const uint32_t num_threads = std::thread::hardware_concurrency();
    
    qlm::Timer<qlm::usec> timer;

    // input array to be summed
    int* arr = new int[arr_size];
    for (int i = 0; i < arr_size; i++)
    {
        arr[i] = i;
    }

    // single thread code
    timer.Start();
    const long single_th = ArrSum(arr, arr_size);
    timer.End();

    const float single_th_time = timer.Duration();

    // thread pool code
    // create thread pool
    qlm::ThreadPool pool{ num_threads };

    timer.Start();
    const long multi_th = ThreadPool_ArrSum(arr, arr_size, pool);
    timer.End();

    const float multi_th_time = timer.Duration();

    if (multi_th != single_th)
    {
        std::cout << "The results are different!: " << multi_th << " vs " << single_th << "\n";
    }
    else
    {
        std::cout << "The results are the same: " << multi_th << "\n";
    }

    // Output the timings
    std::cout << "Single-threaded time: " << single_th_time << " micro seconds\n";
    std::cout << "Multi-threaded time: " << multi_th_time << " micro seconds\n";

    if (multi_th_time < single_th_time)
    {
        std::cout << "Thread Pool faster by "
                  << ((single_th_time - multi_th_time) / multi_th_time) * 100 << " %\n";
    }
    else
    {
        std::cout << "Thread Pool slower by "
                  << ((multi_th_time -single_th_time) /single_th_time) * 100 << " %\n";
    }
}