#pragma once

#include <windows.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <memory>

namespace fw
{

class Input
{
public:
	Input();
	~Input();

	void initialize(HWND windowHandle);
	void update();
	void lateUpdate();
	
	const DirectX::Mouse* getMouse() const;
	const DirectX::Keyboard* getKeyboard() const;
	const DirectX::Mouse::ButtonStateTracker* getMouseTracker() const;
	const DirectX::Keyboard::KeyboardStateTracker* getKeyboardTracker() const;
	float getDeltaX() const;
	float getDeltaY() const;

private:
	std::unique_ptr<DirectX::Mouse> mouse;
	std::unique_ptr<DirectX::Keyboard> keyboard;
	DirectX::Mouse::ButtonStateTracker mouseTracker;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;
	int lastMouseX = 0;
	int lastMouseY = 0;
};

} // fw