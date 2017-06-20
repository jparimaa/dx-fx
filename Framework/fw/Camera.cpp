#include "Camera.h"

using namespace DirectX;

namespace fw
{

Camera::Camera()
{
}

Camera::~Camera()
{
}

float Camera::getNearClipDistance() const
{
	return nearClipDistance;
}

float Camera::getFarClipDistance() const
{
	return farClipDistance;
}

const XMMATRIX& Camera::updateViewMatrix()
{
	viewMatrix = XMMatrixLookAtLH(transformation.position,
								  transformation.position + transformation.getForward(),
								  transformation.getUp());
	return viewMatrix;
}

const XMMATRIX& Camera::getViewMatrix() const
{
	return viewMatrix;
}

Transformation& Camera::getTransformation()
{
	return transformation;
}

} // fw
