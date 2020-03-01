#include "CameraController.h"
#include "API.h"
#include "Common.h"
#include <DirectXMath.h>
#include <iostream>
#include <algorithm>

using namespace DirectX;

namespace
{
const float ROTATION_LIMIT = XMConvertToRadians(87.0f);

} // namespace

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

void CameraController::setCameraTransformation(Transformation* t)
{
    transformation = t;
}

void CameraController::setMovementSpeed(float s)
{
    movementSpeed = s;
}

void CameraController::setSensitivity(float s)
{
    sensitivity = XMConvertToRadians(s);
}

void CameraController::setResetPosition(std::array<float, 3> position)
{
    resetPosition = position;
}

void CameraController::setResetRotation(std::array<float, 3> rotation)
{
    resetRotation = rotation;
}

void CameraController::update()
{
    if (!transformation)
    {
        printWarning("Camera is not set for camera controller");
        return;
    }

    const Keyboard::State& kb = API::getKeyboardState();
    if (kb.LeftShift)
    {
        return;
    }

    float speed = movementSpeed * API::getTimeDelta();

    if (kb.W)
    {
        transformation->move(transformation->getForward() * speed);
    }
    if (kb.S)
    {
        transformation->move(-transformation->getForward() * speed);
    }
    if (kb.A)
    {
        transformation->move(transformation->getLeft() * speed);
    }
    if (kb.D)
    {
        transformation->move(-transformation->getLeft() * speed);
    }

    if (API::getMouseState().leftButton == Mouse::ButtonStateTracker::HELD)
    {
        transformation->rotate(Transformation::UP, API::getDeltaX() * sensitivity);
        transformation->rotate(Transformation::LEFT, -API::getDeltaY() * sensitivity);
        XMFLOAT3 r;
        XMStoreFloat3(&r, transformation->rotation);
        r.x = (std::min)(r.x, ROTATION_LIMIT);
        r.x = (std::max)(r.x, -ROTATION_LIMIT);
        transformation->rotation = XMLoadFloat3(&r);
    }

    if (API::isKeyReleased(Keyboard::R))
    {
        transformation->rotation = XMVectorSet(resetRotation[0], resetRotation[1], resetRotation[2], 0.0f);
        transformation->position = XMVectorSet(resetPosition[0], resetPosition[1], resetPosition[2], 0.0f);
    }
}

} // namespace fw
