#include "API.h"
#include "Framework.h"

namespace fw
{

API::API(Framework* fw) :
	framework(fw)
{
}

API::~API()
{
}

float API::getWindowRatio() const
{
	const Window& w = framework->getWindow();
	float width = static_cast<float>(w.getWidth());
	float height = static_cast<float>(w.getHeight());
	return width / height;
}

HWND API::getWindowHandle() const
{
	return framework->getWindow().getHandle();
}

float API::getTimeSinceStart() const
{
	return framework->getTimer().getTimeSinceStart();
}

float API::getTimeDelta() const
{
	return framework->getTimer().getTimeDelta();
}

void API::quit()
{
	framework->quit();
}

}