#include "PerspectiveCamera.h"

using namespace DirectX;

namespace fw
{

PerspectiveCamera::PerspectiveCamera()
{
	updateViewMatrix();
	updateProjectionMatrix();
}

PerspectiveCamera::~PerspectiveCamera()
{
}

const XMMATRIX& PerspectiveCamera::updateProjectionMatrix()
{
	projectionMatrix = XMMatrixPerspectiveFovLH(FOV, ratio, getNearClipDistance(), getFarClipDistance());
	return projectionMatrix;
}

const XMMATRIX& PerspectiveCamera::getProjectionMatrix() const
{
	return projectionMatrix;
}

} // fw
