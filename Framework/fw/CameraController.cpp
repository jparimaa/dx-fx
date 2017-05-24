#include "CameraController.h"
#include "Transformation.h"
#include "API.h"
#include <DirectXMath.h>
#include <iostream>
#include <algorithm>

using namespace DirectX;

namespace
{

const float ROTATION_LIMIT = XMConvertToRadians(87.0f);

} // anonymous

namespace fw
{

CameraController::CameraController()
{
	setMovementSpeed(1.0f);
	setSensitivity(0.5f);
}

CameraController::~CameraController()
{
}

void CameraController::setCamera(Camera* c)
{
	camera = c;
}

void CameraController::setMovementSpeed(float s)
{
	movementSpeed = s;
}

void CameraController::setSensitivity(float s)
{
	sensitivity = XMConvertToRadians(s);
}

void CameraController::update()
{
	if (!camera) {
		std::cerr << "WARNING: Camera is not set for camera controller\n";
		return;
	}

	Transformation& t = camera->getTransformation();
	float speed = movementSpeed * API::getTimeDelta();
	const Keyboard::State& kb = API::getKeyboardState();
	if (kb.W) {
		t.move(t.getForward() * speed);
	}
	if (kb.S) {
		t.move(-t.getForward() * speed);
	}
	if (kb.A) {
		t.move(t.getLeft() * speed);
	}
	if (kb.D) {
		t.move(-t.getLeft() * speed);
	}

	if (API::getMouseState().leftButton == Mouse::ButtonStateTracker::HELD) {
		t.rotate(Transformation::UP, API::getDeltaX() * sensitivity);
		t.rotate(Transformation::LEFT, -API::getDeltaY() * sensitivity);
		XMFLOAT3 r;
		XMStoreFloat3(&r, t.rotation);
		r.x = std::min(r.x, ROTATION_LIMIT);
		r.x = std::max(r.x, -ROTATION_LIMIT);
		t.rotation = XMLoadFloat3(&r);
	}

	if (API::isKeyReleased(Keyboard::R)) {
		t.rotation = XMVectorZero();
		t.position = XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
	}
}

} // fw
