#pragma once

#include "Transformation.h"
#include <windows.h>
#include <xnamath.h>

namespace fw
{

class Camera
{
public:
	Camera();
	~Camera();

	const XMMATRIX& updateViewMatrix();
	const XMMATRIX& updateProjectionMatrix();

	const XMMATRIX& getViewMatrix();
	const XMMATRIX& getProjectionMatrix();

	Transformation& getTransformation();

private:
	float FOV = XM_PIDIV2;
	float ratio = 1.33333f;
	float nearClipDistance = 0.1f;
	float farClipDistance = 100.0f;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	Transformation transformation;
};

} // fw
