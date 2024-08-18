#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace qlm
{
	template<typename T>
	class ThreadSafeQueue
	{
	private:
		std::queue<T> data_queue;
		mutable std::mutex mut;
		std::condition_variable cv;
	public:
		ThreadSafeQueue() = default;

		ThreadSafeQueue(int len) : data_queue(len)
		{}

		// disable copy and move constructors and the corresponding assignment operators
		ThreadSafeQueue(const ThreadSafeQueue&) = delete;
		ThreadSafeQueue(ThreadSafeQueue&&) = delete;
		ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
		ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;
	public:
		void Push(T new_value)
		{
			std::lock_guard<std::mutex> lk(mut);
			data_queue.push(std::move(new_value));
			cv.notify_one();
		}

		void WaitPop(T& value)
		{
			std::unique_lock<std::mutex> lk(mut);
			cv.wait(lk, [this]() {return !data_queue.empty(); });
			value = std::move(data_queue.front());
			data_queue.pop();
		}

		bool TryPop(T& value)
		{
			std::lock_guard<std::mutex> lk(mut);
			if (data_queue.empty())
				return false;
			value = std::move(data_queue.front());
			data_queue.pop();
			return true;
		}

		bool Empty() const
		{
			std::lock_guard<std::mutex> lk(mut);
			return data_queue.empty();
		}

		int Size() const 
		{
			std::lock_guard<std::mutex> lk(mut);
			return data_queue.size();
		}
	};
}