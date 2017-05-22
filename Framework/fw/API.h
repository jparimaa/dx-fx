#pragma once

#include "Timer.h"
#include "Input.h"
#include "Device.h"
#include <Keyboard.h>
#include <d3d11.h>
#include <windows.h>

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
	
	ID3D11DepthStencilView* getDepthStencilView();

	void quit();

private:
	Framework* framework = nullptr;
	const Timer* timer = nullptr;
	const Input* input = nullptr;
	Device* device = nullptr;
};

}