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
	
	const DirectX::Mouse* getMouse() const;
	const DirectX::Keyboard* getKeyboard() const;
	const DirectX::Mouse::ButtonStateTracker* getMouseState() const;
	const DirectX::Keyboard::KeyboardStateTracker* getKeyboardState() const;

private:
	std::unique_ptr<DirectX::Mouse> mouse;
	std::unique_ptr<DirectX::Keyboard> keyboard;
	DirectX::Mouse::ButtonStateTracker mouseState;
	DirectX::Keyboard::KeyboardStateTracker keyboardState;
};

} // fw