#pragma once

#include <chrono>

namespace qlm
{
	/**
	 * @brief Alias for a high-resolution time point.
	 */
	using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

	/**
	 * @brief Alias for a duration in seconds.
	 */
	using sec = std::chrono::seconds;

	/**
	 * @brief Alias for a duration in milliseconds.
	 */
	using msec = std::chrono::milliseconds;

	/**
	 * @brief Alias for a duration in microseconds.
	 */
	using usec = std::chrono::microseconds;

	/**
	 * @brief Alias for a duration in nanoseconds.
	 */
	using nsec = std::chrono::nanoseconds;

	/**
	 * @brief A timer class template for measuring elapsed time in a specified duration type.
	 * 
	 * @tparam Duration_t The type of duration to measure (e.g., std::chrono::seconds, std::chrono::milliseconds).
	 */
	template<typename Duration_t>
	struct Timer
	{
	private:
		time_point start_time, end_time; ///< The start and end time points.
		Duration_t duration;             ///< The duration between start and end time points.

	public:
		/**
		 * @brief Starts the timer.
		 * 
		 * Records the current time as the start time.
		 */
		void Start()
		{
			start_time = std::chrono::high_resolution_clock::now();
		}

		/**
		 * @brief Ends the timer.
		 * 
		 * Records the current time as the end time and calculates the elapsed duration.
		 */
		void End()
		{
			end_time = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<Duration_t>(end_time - start_time);
		}

		/**
		 * @brief Gets the duration between the start and end time points.
		 * 
		 * @return float The duration in the specified unit (seconds, milliseconds, etc.).
		 */
		float Duration() const
		{
			return duration.count();
		}
	};
}
