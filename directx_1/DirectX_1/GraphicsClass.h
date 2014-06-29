#pragma once
#include <Windows.h>
#include <vector>
#include <d3d11.h>

class GraphicsClass{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass& other);
	~GraphicsClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Render();

private:
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11Texture2D* backBufferPtr;
	ID3D11Texture2D* depthStencilBuffer;
};