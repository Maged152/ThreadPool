#include <iostream>
#include <cmath>
#include <omp.h>
#include <random>
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

long OMP_ArrSum (const int* ptr, const int size)
{
    long res = 0;
    #pragma omp parallel for reduction(+ : res) 
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
    long res = 0;
    for (int i = 0; i < threads; i++)
    {
        res += futures[i].get();
    }

    return res;
}

int main()
{
    std::cout << "Start array_sum example\n";
    constexpr int arr_size = 100000000;
    const uint32_t num_threads = std::thread::hardware_concurrency();
    
    qlm::Timer<qlm::usec> st_timer, tp_timer, omp_timer;

    // input array to be summed
    int* arr = new int[arr_size];
    std::random_device rnd;
    std::mt19937 gen(rnd());
    std::uniform_int_distribution<int> dist(-100, 100);
    for (int i = 0; i < arr_size; i++)
    {
        arr[i] = dist(gen);
    }

    // single thread code
    st_timer.Start();
    const long st_res = ArrSum(arr, arr_size);
    st_timer.End();

    const float st_time = st_timer.Elapsed();

    // omp code
    omp_timer.Start();
    const long omp_res = OMP_ArrSum(arr, arr_size);
    omp_timer.End();

    const float omp_time = omp_timer.Elapsed();

    // multi thread code
    // create thread pool
    qlm::ThreadPool pool{ num_threads };

    tp_timer.Start();
    const long tp_res = ThreadPool_ArrSum(arr, arr_size, pool);
    tp_timer.End();
    
    const float tp_time = tp_timer.Elapsed();

    if (tp_res != st_res)
    {
        std::cout << "The results are different!: " << tp_res << " vs " << st_res << "\n";
    }
    else
    {
        std::cout << "The results are the same: " << tp_res << "\n";
    }

    // Output the timings
    std::cout << "Single-threaded time: " << st_timer.ElapsedString() << "\n";
    std::cout << "Thread pool time: " << tp_timer.ElapsedString() << "\n";
    std::cout << "OMP time: " << omp_timer.ElapsedString() << "\n";

    std::cout << "Thread Pool is " << ((tp_time - st_time) / st_time) * 100 << "% than single thread code\n";
    std::cout << "Thread Pool is " << ((tp_time - omp_time) / omp_time) * 100 << "% than single thread code\n";

    delete[] arr;
}