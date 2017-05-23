#pragma once

#include "API.h"
#include <memory>

namespace fw
{

class Framework;

class Application
{
public:
	Application();
	virtual ~Application();

	Application(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(const Application&) = delete;
	Application& operator=(Application&&) = delete;
	
	virtual bool initialize() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void gui() = 0;
};

} // fw