#pragma once

#include <windows.h>
#include <xnamath.h>

namespace fw
{

class Transformation
{
public:
	static const XMVECTOR FORWARD;
	static const XMVECTOR UP;
	static const XMVECTOR LEFT;

	XMVECTOR position;
	XMVECTOR rotation;
	XMVECTOR scale;

	Transformation();
	~Transformation();

	void move(const XMFLOAT3& translation);
	void move(const XMVECTOR& translation);
	void rotate(const XMFLOAT3& axis, float amount);
	void rotate(const XMVECTOR& axis, float amount);

	XMVECTOR getForward() const;
	XMVECTOR getUp() const;
	XMVECTOR getLeft() const;

	const XMMATRIX& updateWorldMatrix();
	const XMMATRIX& getWorldMatrix();
	
private:
	XMVECTOR forward;
	XMVECTOR up;
	XMVECTOR left;
	XMMATRIX worldMatrix;
};

} // fw
