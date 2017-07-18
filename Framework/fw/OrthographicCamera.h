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

	void setViewBox(float viewLeft, float viewRight, float viewBottom, float viewTop, float viewNearZ, float viewFarZ);

	virtual const DirectX::XMMATRIX& updateProjectionMatrix() final;
	virtual const DirectX::XMMATRIX& getProjectionMatrix() const final;

private:
	float left = -10.0f;
	float right = 10.0f;
	float bottom = -10.0f;
	float top = 10.0f;
	DirectX::XMMATRIX projectionMatrix;
};

} // fw
