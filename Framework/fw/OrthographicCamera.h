#pragma once

#include "Camera.h"
#include "Transformation.h"
#include <DirectXMath.h>

namespace fw
{

class OrthographicCamera : public Camera
{
public:
	OrthographicCamera();
	virtual ~OrthographicCamera() final;

	virtual const DirectX::XMMATRIX& updateProjectionMatrix() final;
	virtual const DirectX::XMMATRIX& getProjectionMatrix() final;

private:
	float width = 800.0f;
	float height = 600.0f;

	DirectX::XMMATRIX projectionMatrix;
};

} // fw
