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

	void setWidth(float w);
	void setHeight(float h);

	virtual const DirectX::XMMATRIX& updateProjectionMatrix() final;
	virtual const DirectX::XMMATRIX& getProjectionMatrix() const final;

private:
	float width = 28.0f;
	float height = 21.0f;

	DirectX::XMMATRIX projectionMatrix;
};

} // fw
