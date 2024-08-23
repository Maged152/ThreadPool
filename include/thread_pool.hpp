#pragma once

#include "thread_safe_queue.hpp"
#include "timer.hpp"
#include <vector>
#include <thread>
#include <functional>
#include <future>
#include <type_traits>

namespace qlm
{
	/**
	 * @brief A thread pool class that manages a pool of worker threads.
	 * 
	 * The `ThreadPool` class allows you to submit tasks that will be executed asynchronously by a fixed number of worker threads.
	 */
	class ThreadPool
	{
	private:
		std::vector<std::thread> workers;                       ///< Vector of worker threads.
		ThreadSafeQueue<std::function<void()>> task_queue;      ///< Queue for storing tasks to be executed by the worker threads.

		std::atomic_bool kill;                                  ///< Flag to indicate if the thread pool should kill all threads immediately.
		std::atomic_bool stop;                                  ///< Flag to indicate if the thread pool should stop processing tasks after current ones finish.

		mutable std::mutex mut;                                 ///< Mutex for synchronizing access to the task queue.
		std::condition_variable cv;                             ///< Condition variable to notify worker threads of new tasks.

		uint32_t thread_count;                                  ///< Number of worker threads in the pool.

	public:
		uint32_t used_threads;                                  ///< Number of threads currently in use.

	private:
		/**
		 * @brief The worker thread function.
		 * 
		 * This function is executed by each worker thread. It waits for tasks to be added to the queue and executes them.
		 */
		void WorkerThread();

	public:
		// Disable copy and move constructors and assignment operators
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;

		/**
		 * @brief Constructs a thread pool with the specified number of threads.
		 * 
		 * @param thread_count The number of threads in the pool. Defaults to the number of hardware concurrency.
		 */
		ThreadPool(const uint32_t thread_count = std::thread::hardware_concurrency());

		/**
		 * @brief Destructor for the thread pool.
		 * 
		 * Joins all worker threads and stops the thread pool.
		 */
		~ThreadPool();

	public:
		/**
		 * @brief Returns the number of worker threads in the pool.
		 * 
		 * @return The number of threads in the pool.
		 */
		uint32_t Size() const;

		/**
		 * @brief Checks if the thread pool is still running.
		 * 
		 * @return true if the thread pool is running; false otherwise.
		 */
		bool Running() const;

		/**
		 * @brief Stops the thread pool and processes all remaining tasks.
		 * 
		 * This function stops the pool from accepting new tasks but allows the currently queued tasks to finish execution.
		 */
		void Stop();

		/**
		 * @brief Stops the thread pool and discards all remaining tasks.
		 * 
		 * This function stops the pool from accepting new tasks and discards all tasks remaining in the queue.
		 */
		void Kill();

		/**
		 * @brief Submits a task to be executed by the thread pool.
		 * 
		 * @tparam F The type of the function or callable object.
		 * @tparam Args The types of the arguments to pass to the function.
		 * @param fun The function or callable object to execute.
		 * @param args The arguments to pass to the function.
		 * @return A `std::future` object representing the result of the task.
		 */
		template <class F, class... Args>
		std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>> Submit(F&& fun, Args&&... args);
	};


	inline void ThreadPool::WorkerThread()
	{
		while (true)
		{
			std::function<void()> task;
			{
				std::unique_lock lk(mut);
				cv.wait(lk, [this]() {return !task_queue.Empty() || kill || stop; });

				if (kill || (stop && task_queue.Empty()))
				{
					return;
				}
				task_queue.WaitPop(task);
			}

			task();
		}
	}

	inline ThreadPool::ThreadPool(const uint32_t thread_count) : thread_count(thread_count), used_threads(thread_count), workers(thread_count), kill(false), stop(false)
	{
		for (int i = 0; i < thread_count; i++) 
		{
			workers[i] = std::move(std::thread(std::bind(&ThreadPool::WorkerThread, this)));
		}
	}

	inline ThreadPool::~ThreadPool()
	{
		stop = true;
		thread_count = 0;
		// notify all threads to finish the remained tasks
		cv.notify_all();
		for (auto& worker : workers)
			worker.join();
	}

	template<class F, class ...Args>
	inline std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>> ThreadPool::Submit(F&& fun, Args && ...args)
	{
		
		using return_type = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(fun), std::forward<Args>(args)...));

		std::future<return_type> res = task->get_future();

		{
			std::unique_lock lock(mut);

			if (stop || kill)
				throw std::runtime_error("The thread pool has been stop.");

			task_queue.Push([task]() -> void { (*task)(); });
		}
		cv.notify_one();

		return res;
	}

	inline void ThreadPool::Stop()
	{
		stop = true;
		thread_count = 0;
		// notify all threads to finish the remained tasks
		cv.notify_all();
	}

	inline void ThreadPool::Kill()
	{
		kill = true;
		thread_count = 0;
		// notify all threads to finish the remained tasks
		cv.notify_all();
	}

	inline uint32_t qlm::ThreadPool::Size() const
	{
		return thread_count;
	}

	inline bool qlm::ThreadPool::Running() const
	{
		return !stop && !kill;
	}
}