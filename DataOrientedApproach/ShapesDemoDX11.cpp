#include "ShapesDemoDX11.h"

using namespace std;
using namespace Microsoft::WRL;

ShapesDemo::ShapesDemo(UINT bufferCount, string name, LONG width, LONG height) : Graphics{ bufferCount, name, width, height }
{

}

void ShapesDemo::render()
{
	UINT presentCount;
	swapChain->GetLastPresentCount(&presentCount);

	ID3D11RenderTargetView* currRtv{ renderTargetViews[presentCount % bufferCount].Get() };

	static const FLOAT clearColor[]{ 0.4f, 0.4f, 0.8f, 1.0f };
	context->ClearRenderTargetView(currRtv, clearColor);
	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->OMSetRenderTargets(1, &currRtv, depthStencilView.Get());

	swapChain->Present(0, 0);
}