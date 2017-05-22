#include "API.h"
#include "Framework.h"

namespace fw
{

API::API(Framework* fw) :
	framework(fw),
	timer(fw->getTimer()),
	input(fw->getInput()),
	device(fw->getDevice())
{
}

API::~API()
{
}

float API::getWindowRatio() const
{
	const Window& w = *framework->getWindow();
	float width = static_cast<float>(w.getWidth());
	float height = static_cast<float>(w.getHeight());
	return width / height;
}

HWND API::getWindowHandle() const
{
	return framework->getWindow()->getHandle();
}

float API::getTimeSinceStart() const
{
	return timer->getTimeSinceStart();
}

float API::getTimeDelta() const
{
	return timer->getTimeDelta();
}

bool API::isKeyReleased(DirectX::Keyboard::Keys k) const
{
	return input->getKeyboardState()->IsKeyReleased(k);
}

int API::getMousePosX() const
{
	return input->getMouse()->GetState().x;
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