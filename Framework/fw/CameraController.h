#pragma once

#include "Camera.h"

namespace fw
{

class CameraController
{
public:
	CameraController();
	~CameraController();

	void setCamera(Camera* c);
	void setMovementSpeed(float s);
	void setSensitivity(float s);
	void update();

private:
	Camera* camera = nullptr;
	float movementSpeed;
	float sensitivity;
};

} // fw