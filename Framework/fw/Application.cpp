#include "Application.h"

namespace fw
{

Application::Application()
{
}

Application::~Application()
{
}

void Application::initializeAPI(Framework* framework)
{
	if (api.get()) {
		return;
	}
	api.reset(new API(framework));
}

API* Application::getAPI()
{
	return api.get();
}

} // fw
