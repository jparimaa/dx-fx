#pragma once

#include "Transformation.h"
#include <array>

namespace fw
{

class CameraController
{
public:
	CameraController();
	~CameraController();

	void setCameraTransformation(Transformation* t);
	void setMovementSpeed(float s);
	void setSensitivity(float s);
	void setResetPosition(std::array<float, 3> position);
	void setResetRotation(std::array<float, 3> rotation);
	void update();

private:
	Transformation* transformation = nullptr;
	float movementSpeed;
	float sensitivity;
	std::array<float, 3> resetPosition = {0.0f, 0.0f, 0.0f};
	std::array<float, 3> resetRotation = {0.0f, 0.0f, 0.0f};
};

} // fw