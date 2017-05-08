#pragma once

#include <windows.h>

namespace fw
{

class Timer
{
public:
	Timer();
	~Timer();

	bool start();
	void tick();
	float getTimeSinceStart() const;
	float getTimeDelta() const;

private:
	float frequency = 0.0f;
	__int64 startTick = 0;
	__int64 lastTick = 0;
	float timeSinceStart = 0.0f;
	float timeDelta = 0.0f;
};

}