#pragma once

#include <Windows.h>

namespace fw
{

class Framework;

class API
{
public:
	API(Framework* fw);
	~API();
	
	float getWindowRatio() const;
	HWND getWindowHandle() const;
	float getTimeSinceStart() const;
	float getTimeDelta() const;
	void quit();

private:
	Framework* framework = nullptr;
};

}