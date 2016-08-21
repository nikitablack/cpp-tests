#pragma once

#include <dxgi1_2.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <vector>
#include <cstdint>
#include <string>
#include <memory>

class GraphicsDX11
{
public:
	GraphicsDX11(UINT bufferCount, std::string name, LONG width, LONG height);

protected:
	void createWindow(std::string name, LONG width, LONG height);
	void createFactory();
	void createDeviceAndContext();
	void createSwapChain();
	void createRenderTargetViews();

protected:
	std::shared_ptr<class Window> window;
	UINT bufferCount;
	Microsoft::WRL::ComPtr<IDXGIFactory1> factory;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> renderTargetViews;
};