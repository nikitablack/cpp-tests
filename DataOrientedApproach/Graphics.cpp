#include "Graphics.h"
#include <stdexcept>
#include "Window.h"

using namespace std;
using namespace Microsoft::WRL;

Graphics::Graphics(UINT bufferCount, string name, LONG width, LONG height) : bufferCount{ bufferCount }
{
	createWindow(name, width, height);
	createFactory();
	createDeviceAndContext();
	createSwapChain();
	createRenderTargetViews();
	createDepthStencilAndView();
}

void Graphics::createWindow(string name, LONG width, LONG height)
{
	window = make_shared<Window>(width, height, name.c_str());
}

void Graphics::createFactory()
{
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(factory.ReleaseAndGetAddressOf()))))
	{
		throw(runtime_error{ "Error creating IDXGIFactory." });
	}
}

void Graphics::createDeviceAndContext()
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

void Graphics::createSwapChain()
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

void Graphics::createRenderTargetViews()
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

void Graphics::createDepthStencilAndView()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChain->GetDesc(&swapChainDesc);

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = swapChainDesc.BufferDesc.Width;
	desc.Height = swapChainDesc.BufferDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc = swapChainDesc.SampleDesc;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&desc, NULL, depthStencil.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating depth stencil texture." });
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
	ZeroMemory(&viewDesc, sizeof(viewDesc));
	viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipSlice = 0;
	viewDesc.Flags = 0;

	if (FAILED(device->CreateDepthStencilView(depthStencil.Get(), &viewDesc, depthStencilView.ReleaseAndGetAddressOf())))
	{
		throw(runtime_error{ "Error creating depth stencil view." });
	}
}

/*ID3D11Device* Graphics::getDevice()
{
	return device.Get();
}

ComPtr<ID3D11Device> Graphics::getDeviceCom()
{
	return device;
}

ID3D11DeviceContext* Graphics::getContext()
{
	return context.Get();
}

IDXGISwapChain* Graphics::getSwapChain()
{
	return swapChain.Get();
}

ID3D11RenderTargetView* Graphics::getRenderTargetView(uint32_t ind)
{
	return renderTargetViews[ind].Get();
}

ID3D11DepthStencilView* Graphics::getDepthStencilView()
{
	return depthStencilView.Get();
}*/