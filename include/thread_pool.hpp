#pragma once

#include "thread_safe_queue.hpp"
#include <vector>
#include <thread>
#include <functional>
#include <future>
#include <type_traits>

namespace qlm
{
	class ThreadPool
	{
	private:
		std::vector<std::thread> workers;
		ThreadSafeQueue<std::function<void()>> task_queue;

		std::atomic_bool kill;
		std::atomic_bool stop;

		mutable std::mutex mut;
		std::condition_variable cv;
		
		size_t thread_count;
	public:
		size_t used_threads;
		
	private:
		void WorkerThread();

	public:
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;

		ThreadPool(const size_t thread_count = std::thread::hardware_concurrency());

		~ThreadPool();
	public:
		// number of working threads
		size_t Size() const;
		// are the threads active
		bool Running() const;
		// stop and process all delegated tasks
		void Stop();
		// stop and drop all tasks remained in queue
		void Kill();
		// submit task
		template <class F, class... Args>
		std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>  Submit(F&& fun, Args &&...args);
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

	inline ThreadPool::ThreadPool(const size_t thread_count) : thread_count(thread_count), used_threads(thread_count), workers(thread_count), kill(false), stop(false)
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

	inline size_t qlm::ThreadPool::Size() const
	{
		return thread_count;
	}

	inline bool qlm::ThreadPool::Running() const
	{
		return !stop && !kill;
	}
}