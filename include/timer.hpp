#pragma once

#include <chrono>

namespace qlm
{
	using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
	using sec = std::chrono::seconds;
	using msec = std::chrono::milliseconds;
	using usec = std::chrono::microseconds;
	using nsec = std::chrono::nanoseconds;

	template<typename Duration_t>
	struct Timer
	{
	private:
		time_point start_time, end_time;
		Duration_t duration;
	public:
		void Start()
		{
			start_time = std::chrono::high_resolution_clock::now();
		}
		void End()
		{
			end_time = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<Duration_t>(end_time - start_time);
		}
		float Duration() const
		{
			return duration.count();
		}
	};
}
