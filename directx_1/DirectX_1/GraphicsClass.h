#pragma once

#pragma comment (lib, "D3DCompiler.lib")

#include <Windows.h>
#include <vector>
#include <d3d11.h>
#include "Globals.h"
#include <d3dcompiler.h>

class GraphicsClass{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass& other);
	~GraphicsClass();

	bool Initialize(const HWND &hwnd);
	void Shutdown();
	bool Render();

private:
	void GetAdapters(std::vector <IDXGIAdapter*>& adapters, IDXGIFactory* factory);
	void GetNumeratorAndDenominator(UINT& numerator, UINT& denominator);
	bool CreateDeviceAndSwapChain(UINT& numerator, UINT& denominator, const HWND& hWnd);
	bool CreateBackBufferAndView();
	bool CreateDepthBufferAndView();
	bool CreateDepthStencilState();
	bool CreateRasterizerState();

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