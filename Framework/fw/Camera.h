#pragma once

#include "Transformation.h"
#include <DirectXMath.h>

namespace fw
{

class Camera
{
public:
	Camera();
	~Camera();

	const DirectX::XMMATRIX& updateViewMatrix();
	const DirectX::XMMATRIX& updateProjectionMatrix();

	const DirectX::XMMATRIX& getViewMatrix();
	const DirectX::XMMATRIX& getProjectionMatrix();

	Transformation& getTransformation();

private:
	float FOV = DirectX::XM_PIDIV4;
	float ratio = 1.33333f;
	float nearClipDistance = 0.1f;
	float farClipDistance = 100.0f;

	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
	Transformation transformation;
};

} // fw
