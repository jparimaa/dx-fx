#pragma once

#include "Camera.h"
#include "Transformation.h"
#include <DirectXMath.h>
#include <array>

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

	std::array<DirectX::XMFLOAT3, 8> getFrustumCorners() const;

private:
	float left = -10.0f;
	float right = 10.0f;
	float bottom = -10.0f;
	float top = 10.0f;
	DirectX::XMMATRIX projectionMatrix;
};

} // fw
