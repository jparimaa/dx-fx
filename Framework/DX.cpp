#include "DX.h"

namespace dfx
{

ID3D11DeviceContext* DX::context = nullptr;
ID3D11RenderTargetView* DX::renderTargetView = nullptr;
IDXGISwapChain* DX::swapChain = nullptr;
ID3D11Device* DX::device = nullptr;

} // fx