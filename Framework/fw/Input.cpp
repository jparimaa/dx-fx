#include "Input.h"

namespace fw
{

Input::Input()
{
}

Input::~Input()
{
}

void Input::initialize(HWND windowHandle)
{
	mouse.reset(new DirectX::Mouse);
	mouse->SetWindow(windowHandle);
	keyboard.reset(new DirectX::Keyboard);
}

void Input::update()
{
	mouseState.Update(mouse->GetState());
	keyboardState.Update(keyboard->GetState());
}

const DirectX::Mouse* Input::getMouse() const
{
	return mouse.get();
}

const DirectX::Keyboard* Input::getKeyboard() const
{
	return keyboard.get();
}

const DirectX::Mouse::ButtonStateTracker* Input::getMouseState() const
{
	return &mouseState;
}

const DirectX::Keyboard::KeyboardStateTracker* Input::getKeyboardState() const
{
	return &keyboardState;
}

}