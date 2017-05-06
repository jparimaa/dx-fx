#include "Camera.h"

namespace fw
{

Camera::Camera()
{
	updateViewMatrix();
	updateProjectionMatrix();
}

Camera::~Camera()
{
}

const XMMATRIX& Camera::updateViewMatrix()
{
	viewMatrix = XMMatrixLookAtLH(transformation.position,
								  transformation.position + transformation.getForward(),
								  transformation.getUp());
	return viewMatrix;
}

const XMMATRIX& Camera::updateProjectionMatrix()
{
	projectionMatrix = XMMatrixPerspectiveFovLH(FOV, ratio, nearClipDistance, farClipDistance);
	return projectionMatrix;
}

const XMMATRIX& Camera::getViewMatrix()
{
	return viewMatrix;
}

const XMMATRIX& Camera::getProjectionMatrix()
{
	return projectionMatrix;
}

Transformation& Camera::getTransformation()
{
	return transformation;
}

} // fw
