#include "API.h"
#include "Framework.h"

namespace fw
{

Framework* API::framework = nullptr;
Device* API::device = nullptr;
const Timer* API::timer = nullptr;
const Input* API::input = nullptr;

void API::initialize(Framework* fw)
{
	if (framework) {
		return;
	}
	framework = fw;
	timer = fw->getTimer();
	input = fw->getInput();
	device = fw->getDevice();
}

float API::getWindowRatio()
{
	const Window& w = *(framework->getWindow());
	float width = static_cast<float>(w.getWidth());
	float height = static_cast<float>(w.getHeight());
	return width / height;
}

HWND API::getWindowHandle()
{
	return framework->getWindow()->getHandle();
}

float API::getTimeSinceStart()
{
	return timer->getTimeSinceStart();
}

float API::getTimeDelta()
{
	return timer->getTimeDelta();
}

DirectX::Keyboard::State API::getKeyboardState()
{
	return input->getKeyboard()->GetState();
}

DirectX::Mouse::State API::getMouseState()
{
	return input->getMouse()->GetState();
}

bool API::isKeyReleased(DirectX::Keyboard::Keys k)
{
	return input->getKeyboardTracker()->IsKeyReleased(k);
}

int API::getMouseX()
{
	return input->getMouse()->GetState().x;
}

int API::getMouseY()
{
	return input->getMouse()->GetState().y;
}

float API::getDeltaX()
{
	return input->getDeltaX();
}

float API::getDeltaY()
{
	return input->getDeltaY();
}

ID3D11RenderTargetView* API::getRenderTargerView()
{
	return device->getRenderTargerView();
}

ID3D11DepthStencilView* API::getDepthStencilView()
{
	return device->getDepthStencilView();
}

void API::quit()
{
	framework->quit();
}

}