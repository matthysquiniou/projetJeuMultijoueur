#pragma once
#include <chrono>

class Clock {
public:
	Clock();
	float GetDeltaTime();
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_previous_time;
};