#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <string>
#include <vector>
#include <wrl/client.h>

class Graphics
{
public:
	Graphics(UINT const bufferCount, class Window const & window);
	virtual ~Graphics();

protected:
	void createFactory();
	void createDeviceAndContext();
	void createSwapChain(class Window const & window);
	void createRenderTargetViews();

protected:
	UINT const _bufferCount;
	Microsoft::WRL::ComPtr<IDXGIFactory1> _factory;
	Microsoft::WRL::ComPtr<ID3D11Device> _device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _swapChain;
	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> _renderTargetViews;
};