#include "pch.h"
#include "Clock.h"

Clock::Clock() {
	m_previous_time = std::chrono::high_resolution_clock::now();
}

float Clock::GetDeltaTime() {
	std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration = now - m_previous_time;
	m_previous_time = now;
	return duration.count();
}