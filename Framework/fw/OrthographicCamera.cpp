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

std::array<DirectX::XMFLOAT3, 8> OrthographicCamera::getFrustumCorners() const
{
	float near = getNearClipDistance();
	float far = getFarClipDistance();
	std::array<DirectX::XMVECTOR, 8> cornerVectors = {
		DirectX::XMVectorSet(left,  top,    near, 1.0f),
		DirectX::XMVectorSet(right, top,    near, 1.0f),
		DirectX::XMVectorSet(right, bottom, near, 1.0f),
		DirectX::XMVectorSet(left,  bottom, near, 1.0f),
		DirectX::XMVectorSet(left,  top,    far,  1.0f),
		DirectX::XMVectorSet(right, top,    far,  1.0f),
		DirectX::XMVectorSet(right, bottom, far,  1.0f),
		DirectX::XMVectorSet(left,  bottom, far,  1.0f)
	};
	
	std::array<DirectX::XMFLOAT3, 8> corners;
	DirectX::XMMATRIX inverse = DirectX::XMMatrixInverse(nullptr, getViewMatrix());
	for (int i = 0; i < 8; ++i) {
		DirectX::XMStoreFloat3(&corners[i], DirectX::XMVector4Transform(cornerVectors[i], inverse));
	}

	return corners;
}

} // fw
