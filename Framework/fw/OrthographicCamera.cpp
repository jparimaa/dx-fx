#include "OrthographicCamera.h"

using namespace DirectX;

namespace fw
{

OrthographicCamera::OrthographicCamera()
{
	updateViewMatrix();
	updateProjectionMatrix();
}

OrthographicCamera::~OrthographicCamera()
{
}

void OrthographicCamera::setViewBox(float viewLeft, float viewRight, float viewBottom, float viewTop, float viewNearZ, float viewFarZ)
{
	left = viewLeft;
	right = viewRight;
	bottom = viewBottom;
	top = viewTop;
	setNearClipDistance(viewNearZ);
	setFarClipDistance(viewFarZ);
}

const XMMATRIX& OrthographicCamera::updateProjectionMatrix()
{
	projectionMatrix = XMMatrixOrthographicOffCenterLH(left, right, bottom, top, getNearClipDistance(), getFarClipDistance());
	return projectionMatrix;
}

const DirectX::XMMATRIX& OrthographicCamera::getProjectionMatrix() const
{
	return projectionMatrix;
}

} // fw
