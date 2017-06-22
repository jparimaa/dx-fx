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
	virtual const DirectX::XMMATRIX& getProjectionMatrix() const final;

private:
	float width = 28.0f;
	float height = 21.0f;

	DirectX::XMMATRIX projectionMatrix;
};

} // fw
