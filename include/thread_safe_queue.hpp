#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace qlm
{
	/**
	 * @brief A thread-safe queue implementation.
	 * 
	 * This class provides a queue that is safe to use in a multithreaded environment.
	 * The queue operations are synchronized using a mutex and a condition variable.
	 * 
	 * @tparam T The type of elements stored in the queue.
	 */
	template<typename T>
	class ThreadSafeQueue
	{
	private:
		std::queue<T> data_queue;           ///< The underlying queue that stores the data.
		mutable std::mutex mut;             ///< Mutex to protect access to the queue.
		std::condition_variable cv;         ///< Condition variable to manage waiting for elements.

	public:
		/**
		 * @brief Default constructor for ThreadSafeQueue.
		 * 
		 * Initializes an empty queue.
		 */
		ThreadSafeQueue() = default;

		/**
		 * @brief Constructs a ThreadSafeQueue with a predefined length.
		 * 
		 * @param len The initial length of the queue.
		 */
		ThreadSafeQueue(int len) : data_queue(len)
		{}

		// Disable copy and move constructors and the corresponding assignment operators
		ThreadSafeQueue(const ThreadSafeQueue&) = delete;
		ThreadSafeQueue(ThreadSafeQueue&&) = delete;
		ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
		ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

	public:
		/**
		 * @brief Pushes a new value into the queue.
		 * 
		 * This method locks the queue, adds the new value, and then notifies one waiting thread.
		 * 
		 * @param new_value The value to be added to the queue.
		 */
		void Push(T new_value)
		{
			std::lock_guard<std::mutex> lk(mut);
			data_queue.push(std::move(new_value));
			cv.notify_one();
		}

		/**
		 * @brief Waits for and pops the front value from the queue.
		 * 
		 * This method blocks until a value is available in the queue.
		 * It then removes the value from the queue and stores it in the provided reference.
		 * 
		 * @param value Reference to the variable where the popped value will be stored.
		 */
		void WaitPop(T& value)
		{
			std::unique_lock<std::mutex> lk(mut);
			cv.wait(lk, [this]() {return !data_queue.empty(); });
			value = std::move(data_queue.front());
			data_queue.pop();
		}

		/**
		 * @brief Tries to pop the front value from the queue without blocking.
		 * 
		 * This method attempts to remove the front value from the queue.
		 * If the queue is empty, it returns false. Otherwise, it stores the popped value in the provided reference and returns true.
		 * 
		 * @param value Reference to the variable where the popped value will be stored.
		 * @return true if a value was successfully popped; false if the queue was empty.
		 */
		bool TryPop(T& value)
		{
			std::lock_guard<std::mutex> lk(mut);
			if (data_queue.empty())
				return false;
			value = std::move(data_queue.front());
			data_queue.pop();
			return true;
		}

		/**
		 * @brief Checks if the queue is empty.
		 * 
		 * This method returns true if the queue contains no elements.
		 * 
		 * @return true if the queue is empty; false otherwise.
		 */
		bool Empty() const
		{
			std::lock_guard<std::mutex> lk(mut);
			return data_queue.empty();
		}

		/**
		 * @brief Returns the number of elements in the queue.
		 * 
		 * This method returns the current size of the queue.
		 * 
		 * @return The number of elements in the queue.
		 */
		int Size() const 
		{
			std::lock_guard<std::mutex> lk(mut);
			return data_queue.size();
		}
	};
}
