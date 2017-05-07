#pragma once

namespace fw
{

class Framework;

class API
{
public:
	API(Framework* fw);
	~API();
	
	float getWindowRatio() const;

private:
	Framework* framework = nullptr;
};

}