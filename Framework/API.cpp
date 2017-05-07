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
	Window& w = *framework->getWindow();
	float width = static_cast<float>(w.getWidth());
	float height = static_cast<float>(w.getHeight());
	return width / height;
}

}