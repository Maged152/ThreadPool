#pragma once

#include <chrono>
#include <string>

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
	 * @tparam duration_t The type of duration to measure (e.g., std::chrono::seconds, std::chrono::milliseconds).
	 */
	template<typename duration_t>
	struct Timer
	{
	private:
		time_point start_time, end_time; ///< The start and end time points.
		duration_t elapsed_time;         ///< The elapsed time between start and end time points.

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
			elapsed_time = std::chrono::duration_cast<duration_t>(end_time - start_time);
		}

		/**
		 * @brief Gets the elapsed time between the start and end time points.
		 * 
		 * @return float The elapsed time in the specified unit (seconds, milliseconds, etc.).
		 */
		float Elapsed() const
		{
			return elapsed_time.count();
		}

		/**
		 * @brief Gets the string representation of the timer type (e.g., seconds, milliseconds).
		 * 
		 * @return const char* The elapsed time type as a string.
		 */
		const char* GetType() const
		{
			if constexpr (std::is_same<duration_t, sec>::value)
			{
				return " sec";
			}
			else if constexpr (std::is_same<duration_t, msec>::value)
			{
				return " msec";
			}
			else if constexpr (std::is_same<duration_t, usec>::value)
			{
				return " usec";
			}
			else
			{
				return " nsec";
			}
		}

		/**
		 * @brief Returns a string with the elapsed time and its type.
		 * 
		 * Combines the elapsed time value and its type (e.g., "100 msec").
		 * 
		 * @return std::string A string containing the elapsed time and its type.
		 */
		std::string ElapsedString() const
		{
			return std::to_string(this->Elapsed()) + GetType();
		}
	};
}
