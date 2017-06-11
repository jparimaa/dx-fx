#pragma once

#include "Camera.h"
#include <array>

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
	void setResetPosition(std::array<float, 3> position);
	void setResetRotation(std::array<float, 3> rotation);
	void update();

private:
	Camera* camera = nullptr;
	float movementSpeed;
	float sensitivity;
	std::array<float, 3> resetPosition = {0.0f, 0.0f, 0.0f};
	std::array<float, 3> resetRotation = {0.0f, 0.0f, 0.0f};
};

} // fw