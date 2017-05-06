#pragma once

#include <windows.h>
#include <xnamath.h>

namespace fw
{

class Transformation
{
public:
	static const XMFLOAT3 FORWARD;
	static const XMFLOAT3 UP;
	static const XMFLOAT3 LEFT;

	XMVECTOR position;
	XMVECTOR rotation;
	XMVECTOR scale;

	Transformation();
	~Transformation();

	void move(const XMFLOAT3& translation);
	void rotate(const XMFLOAT3& axis, float amount);

	XMVECTOR getForward() const;
	XMVECTOR getUp() const;
	XMVECTOR getLeft() const;

	const XMMATRIX& updateWorldMatrix();
	const XMMATRIX& getWorldMatrix();
	
private:
	static const XMVECTOR FORWARD_VEC;
	static const XMVECTOR UP_VEC;
	static const XMVECTOR LEFT_VEC;

	XMVECTOR forward;
	XMVECTOR up;
	XMVECTOR left;
	XMMATRIX worldMatrix;
};

} // fw
