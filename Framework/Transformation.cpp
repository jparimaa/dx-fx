#include "Transformation.h"

namespace fw
{

const XMFLOAT3 Transformation::FORWARD = XMFLOAT3(0.0f, 0.0f, 1.0f);
const XMFLOAT3 Transformation::UP = XMFLOAT3(0.0f, 1.0f, 0.0f);
const XMFLOAT3 Transformation::LEFT = XMFLOAT3(1.0f, 0.0f, 0.0f);

const XMVECTOR Transformation::FORWARD_VEC = XMVectorSet(FORWARD.x, FORWARD.y, FORWARD.z, 0.0f);
const XMVECTOR Transformation::UP_VEC = XMVectorSet(UP.x, UP.y, UP.z, 0.0f);
const XMVECTOR Transformation::LEFT_VEC = XMVectorSet(LEFT.x, LEFT.y, LEFT.z, 0.0f);

Transformation::Transformation() :
	position(XMVectorZero()),
	rotation(XMVectorZero()),
	scale(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f)),
	forward(XMLoadFloat3(&FORWARD)),
	up(XMLoadFloat3(&UP)),
	left(XMLoadFloat3(&LEFT))
{
}

Transformation::~Transformation()
{
}

void Transformation::move(const XMFLOAT3& translation)
{
	position += XMLoadFloat3(&translation);
}

void Transformation::rotate(const XMFLOAT3& axis, float amount)
{
	XMVECTOR a = XMLoadFloat3(&axis);
	rotation += a * amount;
}

XMVECTOR Transformation::getForward() const
{
	return XMVector4Transform(FORWARD_VEC, XMMatrixRotationRollPitchYawFromVector(rotation));
}

XMVECTOR Transformation::getUp() const
{
	return XMVector4Transform(UP_VEC, XMMatrixRotationRollPitchYawFromVector(rotation));
}

XMVECTOR Transformation::getLeft() const
{
	return XMVector4Transform(LEFT_VEC, XMMatrixRotationRollPitchYawFromVector(rotation));
}

const XMMATRIX & Transformation::updateWorldMatrix()
{
	worldMatrix =
		XMMatrixTranslationFromVector(position) *
		XMMatrixRotationRollPitchYawFromVector(rotation) *
		XMMatrixScalingFromVector(scale);
	return worldMatrix;
}

const XMMATRIX & Transformation::getWorldMatrix()
{
	return worldMatrix;
}

} // fw