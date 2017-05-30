#include "RenderData.h"
#include <fw/Common.h>
#include <fw/DX.h>

VertexBuffers::~VertexBuffers()
{
	fw::release(vertexBuffer);
	fw::release(indexBuffer);
}
