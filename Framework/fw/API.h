#pragma once

#include "Timer.h"
#include "Input.h"
#include <windows.h>
#include <Keyboard.h>

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

	bool isKeyReleased(DirectX::Keyboard::Keys k) const;
	int getMousePosX() const;

	void quit();

private:
	Framework* framework = nullptr;
	const Timer* timer = nullptr;
	const Input* input = nullptr;
};

}