#pragma once

#include "Camera.h"
#include "Transformation.h"
#include <DirectXMath.h>

namespace fw
{

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera();
	virtual ~PerspectiveCamera() final;

	virtual const DirectX::XMMATRIX& updateProjectionMatrix() final;
	virtual const DirectX::XMMATRIX& getProjectionMatrix() const final;

private:
	float FOV = DirectX::XM_PIDIV4;
	float ratio = 1.33333f;

	DirectX::XMMATRIX projectionMatrix;
};

} // fw
