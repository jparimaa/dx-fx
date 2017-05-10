#include "Timer.h"
#include <iostream>

namespace fw
{

Timer::Timer()
{
}

Timer::~Timer()
{
}

bool Timer::start()
{
	LARGE_INTEGER counter;
	if (!QueryPerformanceFrequency(&counter)) {
		std::cerr << "WARNING: Could not query performance frequency\n";
		return false;
	}

	frequency = static_cast<float>(counter.QuadPart);

	QueryPerformanceCounter(&counter);
	startTick = counter.QuadPart;
	lastTick = startTick;
	return true;
}

void Timer::tick()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	timeSinceStart = static_cast<float>(counter.QuadPart - startTick) / frequency;
	timeDelta = static_cast<float>(counter.QuadPart - lastTick) / frequency;
	lastTick = counter.QuadPart;
}

float Timer::getTimeSinceStart() const
{
	return timeSinceStart;
}

float Timer::getTimeDelta() const
{
	return timeDelta;
}

}