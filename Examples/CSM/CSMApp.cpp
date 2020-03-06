#include "CSMApp.h"

#include <fw/DX.h>
#include <fw/API.h>
#include <fw/WcharHelper.h>
#include <fw/imgui/imgui.h>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <vector>
#include <iostream>
#include <limits>
#include <algorithm>

namespace
{
float clearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};

std::array<DirectX::XMFLOAT3, 8> getFrustumCorners(const fw::PerspectiveCamera& camera, float nearClip, float farClip)
{
    fw::PerspectiveCamera cam = camera;
    cam.setNearClipDistance(nearClip);
    cam.setFarClipDistance(farClip);
    cam.updateViewMatrix();
    cam.updateProjectionMatrix();
    DirectX::BoundingFrustum frustum;
    DirectX::BoundingFrustum::CreateFromMatrix(frustum, cam.getProjectionMatrix());
    DirectX::XMMATRIX inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, cam.getViewMatrix());
    frustum.Transform(frustum, inverseViewMatrix);
    std::array<DirectX::XMFLOAT3, 8> corners;
    frustum.GetCorners(corners.data());
    return corners;
}
} // namespace

CSMApp::CSMApp()
{
}

CSMApp::~CSMApp()
{
    fw::release(matrixBuffer);
    fw::release(lightMatrixBuffer);
    fw::release(lightBuffer);
    fw::release(depthmapTexture);
    fw::release(depthmapDSV);
    fw::release(depthmapSRV);
}

bool CSMApp::initialize()
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> defaultLayout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    std::string lightingFileStr = ROOT_PATH + std::string("Examples/CSM/lighting.hlsl");
    fw::ToWchar lightingWchar(lightingFileStr);

    if (!lightingVS.create(lightingWchar.getWchar(), "VS", "vs_4_0", defaultLayout))
    {
        return false;
    }

    if (!lightingPS.create(lightingWchar.getWchar(), "PS", "ps_4_0"))
    {
        return false;
    }

    std::string depthmapFileStr = ROOT_PATH + std::string("Examples/CSM/depthmap.hlsl");
    fw::ToWchar depthmapWchar(depthmapFileStr);

    if (!depthmapVS.create(depthmapWchar.getWchar(), "VS", "vs_4_0", defaultLayout))
    {
        return false;
    }

    std::vector<D3D11_INPUT_ELEMENT_DESC> frustumLayout = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    std::string frustumFileStr = ROOT_PATH + std::string("Examples/CSM/frustum.hlsl");
    fw::ToWchar frustumWchar(frustumFileStr);

    if (!frustumVS.create(frustumWchar.getWchar(), "VS", "vs_4_0", frustumLayout))
    {
        return false;
    }

    if (!frustumPS.create(frustumWchar.getWchar(), "PS", "ps_4_0"))
    {
        return false;
    }

    if (!createDepthmap())
    {
        return false;
    }

    if (!createBuffer<MatrixData>(&matrixBuffer))
    {
        return false;
    }

    if (!createBuffer<LightMatrixData>(&lightMatrixBuffer, 3))
    {
        return false;
    }

    if (!createBuffer<DirectionalLightData>(&lightBuffer))
    {
        return false;
    }

    if (!assetManager.getLinearSampler(&samplerLinear))
    {
        return false;
    }

    monkey1.textureView = assetManager.getTextureView(ASSETS_PATH + std::string("checker.png"));
    monkey1.vertexBuffer = assetManager.getVertexBuffer(ASSETS_PATH + std::string("monkey.3ds"));
    monkey1.transformation.position = DirectX::XMVectorSet(4.0f, 1.0f, 0.0f, 0.0f);
    monkey1.transformation.updateWorldMatrix();
    assert(monkey1.textureView != nullptr);
    assert(monkey1.vertexBuffer != nullptr);

    monkey2.textureView = assetManager.getTextureView(ASSETS_PATH + std::string("green_square.png"));
    monkey2.vertexBuffer = assetManager.getVertexBuffer(ASSETS_PATH + std::string("monkey.3ds"));
    monkey2.transformation.position = DirectX::XMVectorSet(-2.0f, 1.0f, 0.0f, 0.0f);
    monkey2.transformation.updateWorldMatrix();
    assert(monkey2.textureView != nullptr);
    assert(monkey2.vertexBuffer != nullptr);

    cube.textureView = assetManager.getTextureView(ASSETS_PATH + std::string("checker.png"));
    cube.vertexBuffer = assetManager.getVertexBuffer(ASSETS_PATH + std::string("cube.obj"));
    cube.transformation.scale = DirectX::XMVectorSet(10.0f, 0.01f, 10.0f, 0.0f);
    cube.transformation.updateWorldMatrix();
    assert(cube.textureView != nullptr);
    assert(cube.vertexBuffer != nullptr);

    viewCamera.getTransformation().position = DirectX::XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
    viewCamera.getTransformation().rotate(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), 0.4f);
    viewCamera.updateViewMatrix();
    cameraController.setCameraTransformation(&viewCamera.getTransformation());
    cameraController.setResetPosition({6.0f, 6.0f, -6.0f});
    cameraController.setResetRotation({0.8f, -0.8f, 0.0f});
    cameraController.setMovementSpeed(4.0f);

    light.transformation.position = DirectX::XMVectorSet(6.0f, 6.0f, -6.0f, 0.0f);
    light.transformation.rotation = DirectX::XMVectorSet(0.8f, -0.8f, 0.0f, 0.0f);
    lightCamera.getTransformation().position = light.transformation.position;
    lightCamera.getTransformation().rotation = light.transformation.rotation;
    lightCamera.updateViewMatrix();
    lightCamera.updateProjectionMatrix();

#ifdef DRAW_DEBUG_LINES
    fw::DX::context->VSSetShader(lightingVS.get(), nullptr, 0);
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    fw::DX::context->Map(lightMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    LightMatrixData* lightMatrixData = (LightMatrixData*)mappedResource.pData;
    lightMatrixData->viewProjection = lightCamera.getViewMatrix() * lightCamera.getProjectionMatrix();
    fw::DX::context->Unmap(lightMatrixBuffer, 0);
    fw::DX::context->VSSetConstantBuffers(1, 1, &lightMatrixBuffer);

    viewCameraFrustumCorners.push_back(getFrustumCorners(viewCamera, viewCamera.getNearClipDistance(), frustumDivisions[0]));
    viewCameraFrustumCorners.push_back(getFrustumCorners(viewCamera, frustumDivisions[0], frustumDivisions[1]));
    viewCameraFrustumCorners.push_back(getFrustumCorners(viewCamera, frustumDivisions[0], viewCamera.getFarClipDistance()));

    fw::OrthographicCamera orthoCam;
    orthoCam = getCascadedCamera(viewCamera.getNearClipDistance(), frustumDivisions[0]);
    shadowMapFrustumCorners.push_back(orthoCam.getFrustumCorners());
    orthoCam = getCascadedCamera(frustumDivisions[0], frustumDivisions[1]);
    shadowMapFrustumCorners.push_back(orthoCam.getFrustumCorners());
    orthoCam = getCascadedCamera(frustumDivisions[1], viewCamera.getFarClipDistance());
    shadowMapFrustumCorners.push_back(orthoCam.getFrustumCorners());
#endif

    for (unsigned int i = 0; i < NUM_CASCADES; ++i)
    {
        viewports[i].Height = static_cast<float>(fw::API::getWindowHeight());
        viewports[i].Width = static_cast<float>(fw::API::getWindowWidth());
        viewports[i].MaxDepth = 1.0f;
        viewports[i].MinDepth = 0.0f;
        viewports[i].TopLeftX = static_cast<float>(fw::API::getWindowWidth() * i);
        viewports[i].TopLeftY = 0.0f;
    }

    cascadeLimits = {5.0f, 15.0f, viewCamera.getFarClipDistance()};

    std::cout << "CSMApp initialization completed\n";

    return true;
}

void CSMApp::update()
{
    if (fw::API::isKeyReleased(DirectX::Keyboard::Escape))
    {
        fw::API::quit();
    }

    monkey1.transformation.rotate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XM_2PI * fw::API::getTimeDelta() * 0.1f);
    monkey1.transformation.updateWorldMatrix();

    cameraController.update();
    viewCamera.updateViewMatrix();

    for (int i = 0; i < NUM_CASCADES; ++i)
    {
        float nearPlane = i == 0 ? viewCamera.getNearClipDistance() : cascadeLimits[i - 1];
        float farPlane = cascadeLimits[i];
        cascadeCameras[i] = getCascadedCamera(nearPlane, farPlane);
    }
}

void CSMApp::render()
{
    renderDepthmaps();
    renderObjects();

#ifdef DRAW_DEBUG_LINES
    drawFrustumLines(viewCameraFrustumCorners[0]);
    drawFrustumLines(viewCameraFrustumCorners[1]);
    drawFrustumLines(viewCameraFrustumCorners[2]);

    drawFrustumLines(shadowMapFrustumCorners[0]);
    drawFrustumLines(shadowMapFrustumCorners[1]);
    drawFrustumLines(shadowMapFrustumCorners[2]);
#endif

    ID3D11ShaderResourceView* nv[] = {nullptr};
    fw::DX::context->PSSetShaderResources(1, 1, nv);
}

void CSMApp::gui()
{
    fw::displayVector("Camera position %.1f, %.1f, %.1f", viewCamera.getTransformation().position);
#ifdef GUI_ROTATION
    fw::displayVector("Camera rotation %.1f %.1f %.1f", camera.getTransformation().rotation);
    fw::displayVector("Camera direction %.1f %.1f %.1f", camera.getTransformation().getForward());
    fw::displayVector("Light direction %.1f %.1f %.1f", light.transformation.getForward());
#endif
    ImGui::ColorEdit3("Light color", light.color.data());
    ImGui::DragFloat("Light power", &light.color[3], 0.01f);
}

template<typename T>
bool CSMApp::createBuffer(ID3D11Buffer** buffer, int numBuffers)
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(T) * numBuffers;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.Usage = D3D11_USAGE_DYNAMIC;

    HRESULT hr = fw::DX::device->CreateBuffer(&bd, nullptr, buffer);
    if (FAILED(hr))
    {
        fw::printError("Failed to create buffer", &hr);
    }
    return true;
}

bool CSMApp::createDepthmap()
{
    D3D11_TEXTURE2D_DESC shadowmapDesc = {
        static_cast<UINT>(fw::API::getWindowWidth() * NUM_CASCADES),
        static_cast<UINT>(fw::API::getWindowHeight()),
        1, // Miplevels
        1, // ArraySize
        DXGI_FORMAT_R32_TYPELESS,
        DXGI_SAMPLE_DESC{1, 0},
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, // Bind flags
        0, // CPU access flag
        0 // Misc flags
    };
    fw::DX::device->CreateTexture2D(&shadowmapDesc, nullptr, &depthmapTexture);

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilVD = {
        DXGI_FORMAT_D32_FLOAT,
        D3D11_DSV_DIMENSION_TEXTURE2D,
        0};
    fw::DX::device->CreateDepthStencilView(depthmapTexture, &depthStencilVD, &depthmapDSV);

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceVD = {
        DXGI_FORMAT_R32_FLOAT,
        D3D11_SRV_DIMENSION_TEXTURE2D,
        0,
        0};
    shaderResourceVD.Texture2D.MipLevels = 1;
    fw::DX::device->CreateShaderResourceView(depthmapTexture, &shaderResourceVD, &depthmapSRV);

    return true;
}

void CSMApp::renderDepthmaps()
{
    fw::DX::context->ClearDepthStencilView(depthmapDSV, D3D11_CLEAR_DEPTH, 1.0, 0);
    ID3D11RenderTargetView* nullView = nullptr;
    // Set a null render target to not render color.
    fw::DX::context->OMSetRenderTargets(1, &nullView, depthmapDSV);
    fw::DX::context->IASetInputLayout(depthmapVS.getVertexLayout());
    fw::DX::context->VSSetShader(depthmapVS.get(), nullptr, 0);
    fw::DX::context->PSSetShader(nullptr, nullptr, 0);

    for (unsigned int i = 0; i < NUM_CASCADES; ++i)
    {
        fw::DX::context->RSSetViewports(1, &viewports[i]);
        const fw::OrthographicCamera& camera = cascadeCameras[i];
        renderObject(monkey1, camera);
        renderObject(monkey2, camera);
        renderObject(cube, camera);
    }

    fw::DX::context->OMSetRenderTargets(1, &nullView, nullptr);
    fw::DX::context->RSSetViewports(1, &viewports[0]);
}

void CSMApp::renderObjects()
{
    ID3D11RenderTargetView* renderTarget = fw::API::getRenderTargetView();
    fw::DX::context->OMSetRenderTargets(1, &renderTarget, fw::API::getDepthStencilView());
    fw::DX::context->ClearRenderTargetView(fw::DX::renderTargetView, clearColor);
    fw::DX::context->ClearDepthStencilView(fw::API::getDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // VS
    fw::DX::context->IASetInputLayout(lightingVS.getVertexLayout());
    fw::DX::context->VSSetShader(lightingVS.get(), nullptr, 0);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    fw::DX::context->Map(lightMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    LightMatrixData* lightMatrixData = (LightMatrixData*)mappedResource.pData;
    for (int i = 0; i < NUM_CASCADES; ++i)
    {
        lightMatrixData->viewProjection = cascadeCameras[i].getViewMatrix() * cascadeCameras[i].getProjectionMatrix();
        ++lightMatrixData;
    }
    fw::DX::context->Unmap(lightMatrixBuffer, 0);
    fw::DX::context->VSSetConstantBuffers(1, 1, &lightMatrixBuffer);

    // PS
    fw::DX::context->PSSetShader(lightingPS.get(), nullptr, 0);
    fw::DX::context->PSSetSamplers(0, 1, &samplerLinear);
    fw::DX::context->PSSetShaderResources(1, 1, &depthmapSRV);

    fw::DX::context->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    DirectionalLightData* lightData = (DirectionalLightData*)mappedResource.pData;
    lightData->position = light.transformation.position;
    lightData->direction = light.transformation.getForward();
    lightData->color = light.color;
    for (int i = 0; i < NUM_CASCADES; ++i)
    {
        lightData->cascadeLimits[i] = cascadeLimits[i];
    }
    fw::DX::context->Unmap(lightBuffer, 0);
    fw::DX::context->PSSetConstantBuffers(0, 1, &lightBuffer);

    renderObject(monkey1, viewCamera);
    renderObject(monkey2, viewCamera);
    renderObject(cube, viewCamera);
}

void CSMApp::renderObject(const RenderData& renderData, const fw::Camera& camera)
{
    fw::VertexBuffer* vb = renderData.vertexBuffer;
    fw::DX::context->IASetVertexBuffers(0, 1, &vb->vertexBuffer, &vb->stride, &vb->offset);
    fw::DX::context->IASetIndexBuffer(vb->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    fw::DX::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    fw::DX::context->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    MatrixData* matrixData = (MatrixData*)mappedResource.pData;
    matrixData->world = renderData.transformation.getWorldMatrix();
    matrixData->view = camera.getViewMatrix();
    matrixData->projection = camera.getProjectionMatrix();
    fw::DX::context->Unmap(matrixBuffer, 0);
    fw::DX::context->VSSetConstantBuffers(0, 1, &matrixBuffer);

    fw::DX::context->PSSetShaderResources(0, 1, &renderData.textureView);
    fw::DX::context->DrawIndexed(static_cast<UINT>(vb->numIndices), 0, 0);
}

fw::OrthographicCamera CSMApp::getCascadedCamera(float nearPlane, float farPlane)
{
    fw::PerspectiveCamera frustumCamera = viewCamera;
    frustumCamera.setNearClipDistance(nearPlane);
    frustumCamera.setFarClipDistance(farPlane);
    frustumCamera.updateProjectionMatrix();

    DirectX::BoundingFrustum frustum;
    DirectX::BoundingFrustum::CreateFromMatrix(frustum, frustumCamera.getProjectionMatrix());
    DirectX::XMMATRIX inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, frustumCamera.getViewMatrix());
    frustum.Transform(frustum, inverseViewMatrix);
    frustum.Transform(frustum, lightCamera.getViewMatrix());

    std::array<DirectX::XMFLOAT3, 8> corners;
    frustum.GetCorners(corners.data());
    DirectX::BoundingBox boundingBox;
    DirectX::BoundingBox::CreateFromPoints(boundingBox, corners.size(), corners.data(), sizeof(DirectX::XMFLOAT3));

    fw::OrthographicCamera cascadeCamera = lightCamera;
    float left = std::numeric_limits<float>::max();
    float right = std::numeric_limits<float>::lowest();
    float bottom = std::numeric_limits<float>::max();
    float top = std::numeric_limits<float>::lowest();
    float farClipPlane = std::numeric_limits<float>::lowest();
    for (int i = 0; i < 8; ++i)
    {
        left = std::min(left, corners[i].x);
        right = std::max(right, corners[i].x);
        bottom = std::min(bottom, corners[i].y);
        top = std::max(top, corners[i].y);
        farClipPlane = std::max(farClipPlane, corners[i].z);
    }
    cascadeCamera.setViewBox(left, right, bottom, top, 0.001f, farClipPlane);
    cascadeCamera.updateViewMatrix();
    cascadeCamera.updateProjectionMatrix();

    return cascadeCamera;
}

// This is slow, use only for debug purposes.
void CSMApp::drawFrustumLines(const std::array<DirectX::XMFLOAT3, 8>& points)
{
    fw::VertexBuffer buffer;
    buffer.releaseOnDestruction = true;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT3) * points.size());
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA data;
    ZeroMemory(&data, sizeof(data));
    data.pSysMem = points.data();

    HRESULT hr = fw::DX::device->CreateBuffer(&bd, &data, &buffer.vertexBuffer);
    if (FAILED(hr))
    {
        fw::printError("ERROR: Failed to create vertex buffer in frustum line drawing", &hr);
        return;
    }

    std::vector<WORD> indices{0, 1, 2, 3, 0, 4, 5, 6, 7, 4, 5, 1, 2, 6, 7, 3};

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(sizeof(WORD) * indices.size());
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    data.pSysMem = indices.data();

    hr = fw::DX::device->CreateBuffer(&bd, &data, &buffer.indexBuffer);
    if (FAILED(hr))
    {
        fw::printError("ERROR: Failed to create index buffer", &hr);
        return;
    }

    buffer.stride = sizeof(DirectX::XMFLOAT3);
    ID3D11RenderTargetView* renderTarget = fw::API::getRenderTargetView();
    fw::DX::context->OMSetRenderTargets(1, &renderTarget, fw::API::getDepthStencilView());
    fw::DX::context->IASetInputLayout(frustumVS.getVertexLayout());
    fw::DX::context->VSSetShader(frustumVS.get(), nullptr, 0);
    fw::DX::context->PSSetShader(frustumPS.get(), nullptr, 0);
    fw::DX::context->IASetVertexBuffers(0, 1, &buffer.vertexBuffer, &buffer.stride, &buffer.offset);
    fw::DX::context->IASetIndexBuffer(buffer.indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    fw::DX::context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    fw::DX::context->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    MatrixData* matrixData = (MatrixData*)mappedResource.pData;
    matrixData->world = DirectX::XMMATRIX();
    matrixData->view = viewCamera.getViewMatrix();
    matrixData->projection = viewCamera.getProjectionMatrix();
    fw::DX::context->Unmap(matrixBuffer, 0);
    fw::DX::context->VSSetConstantBuffers(0, 1, &matrixBuffer);

    fw::DX::context->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}
