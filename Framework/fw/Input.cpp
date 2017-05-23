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
	mouseTracker.Update(mouse->GetState());
	keyboardTracker.Update(keyboard->GetState());
}

const DirectX::Mouse* Input::getMouse() const
{
	return mouse.get();
}

const DirectX::Keyboard* Input::getKeyboard() const
{
	return keyboard.get();
}

const DirectX::Mouse::ButtonStateTracker* Input::getMouseTracker() const
{
	return &mouseTracker;
}

const DirectX::Keyboard::KeyboardStateTracker* Input::getKeyboardTracker() const
{
	return &keyboardTracker;
}

}