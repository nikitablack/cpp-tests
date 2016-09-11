#include "GraphicsDX11.h"
#include <stdexcept>
#include "Window.h"

using namespace std;
using namespace Microsoft::WRL;

GraphicsDX11::GraphicsDX11(UINT bufferCount, string name, LONG width, LONG height) : bufferCount{ bufferCount }, width{ width }, height{ height }
{
	createWindow(name, width, height);
	createFactory();
	createDeviceAndContext();
	createSwapChain();
	createRenderTargetViews();
}

shared_ptr<Window> GraphicsDX11::getWindow()
{
	return window;
}

void GraphicsDX11::createWindow(string name, LONG width, LONG height)
{
	window = make_shared<Window>(width, height, name.c_str());
}

void GraphicsDX11::createFactory()
{
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(factory.ReleaseAndGetAddressOf()))))
	{
		throw(runtime_error{ "Error creating IDXGIFactory." });
	}
}

void GraphicsDX11::createDeviceAndContext()
{
	UINT flags{ D3D11_CREATE_DEVICE_BGRA_SUPPORT };
#if defined(_DEBUG)
	// If the project is in a debug build, enable the debug layer.
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	vector<D3D_FEATURE_LEVEL> featureLevels{ D3D_FEATURE_LEVEL_11_0 };
	if (FAILED(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, featureLevels.data(), static_cast<UINT>(featureLevels.size()), D3D11_SDK_VERSION, device.ReleaseAndGetAddressOf(), NULL, context.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating device and context." });
	}
}

void GraphicsDX11::createSwapChain()
{
	POINT wSize(window->getSize());

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferDesc = { static_cast<UINT>(wSize.x), static_cast<UINT>(wSize.y), { 0, 1 }, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_CENTERED };
	desc.SampleDesc = { 1, 0 };
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = bufferCount;
	desc.OutputWindow = window->getHandle();
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.Flags = 0;

	if (FAILED(factory->CreateSwapChain(device.Get(), &desc, swapChain.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating swap chain." });
	}
}

void GraphicsDX11::createRenderTargetViews()
{
	for (UINT i{ 0 }; i < bufferCount; ++i)
	{
		ComPtr<ID3D11Texture2D> backBuffer;
		if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.ReleaseAndGetAddressOf()))))
		{
			throw(runtime_error{ "Error getting back buffer." });
		}

		ComPtr<ID3D11RenderTargetView> renderTargetView;
		if (FAILED(device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.ReleaseAndGetAddressOf())))
		{
			throw(runtime_error{ "Error creating render target view." });
		}

		renderTargetViews.push_back(renderTargetView);
	}
}
