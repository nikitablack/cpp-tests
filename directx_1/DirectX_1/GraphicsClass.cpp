#include "GraphicsClass.h"
#include <DirectXMath.h>

using namespace DirectX;

struct VertexPosColor
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

VertexPosColor g_Vertices[8] =
{
	{ XMFLOAT3(-1.0f, -1.0f, 0.0f) },
	{ XMFLOAT3(0.0f, 1.0f, 0.0f) },
	{ XMFLOAT3(1.0f, -1.0f, 0.0f) },
};

WORD g_Indicies[3] =
{
	0, 1, 2
};

GraphicsClass::GraphicsClass(){
	device = nullptr;
	context = nullptr;
	swapChain = nullptr;
	renderTargetView = nullptr;
	depthStencilView = nullptr;
	rasterState = nullptr;
	depthStencilState = nullptr;
	backBufferPtr = nullptr;
	depthStencilBuffer = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other){
}

GraphicsClass::~GraphicsClass(){
}

bool GraphicsClass::Initialize(const HWND& hWnd){
	UINT numerator{ 60 };
	UINT denominator{ 1 };

	GetNumeratorAndDenominator(numerator, denominator);

	if (!CreateDeviceAndSwapChain(numerator, denominator, hWnd)){
		return false;
	}

	if (!CreateBackBufferAndView()){
		return false;
	}

	if (!CreateDepthBufferAndView()){
		return false;
	}

	if (!CreateDepthStencilState()){
		return false;
	}

	if (!CreateRasterizerState()){
		return false;
	}

	// Set the depth stencil state.
	context->OMSetDepthStencilState(depthStencilState, 1);

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	// Now set the rasterizer state.
	context->RSSetState(rasterState);

	// Setup the viewport
	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)WIDTH;
	viewport.Height = (FLOAT)HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	context->RSSetViewports(1, &viewport);









	// vertex shader
	ID3DBlob* vsBlob;
	HRESULT result = D3DReadFileToBlob(L"x64/Debug/Vertex.cso", &vsBlob);
	if (FAILED(result)){
		MessageBox(NULL, L"Error reading vertex shader blob", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ID3D11VertexShader* vertexShader;
	result = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
	if (FAILED(result)){
		MessageBox(NULL, L"Error creating vertex shader", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ID3D11InputLayout* inputLayout = nullptr;
	result = device->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
	if (FAILED(result)){
		MessageBox(NULL, L"Error creating input layout", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}


	context->VSSetShader(vertexShader, nullptr, 0);

	vsBlob->Release();
	vsBlob = nullptr;
	vertexShader->Release();
	vertexShader = nullptr;










	// pixel shader
	ID3DBlob* psBlob;
	result = D3DReadFileToBlob(L"x64/Debug/Pixel.cso", &psBlob);
	if (FAILED(result)){
		MessageBox(NULL, L"Error reading pixel shader blob", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ID3D11PixelShader* pixelShader;
	result = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
	if (FAILED(result)){
		MessageBox(NULL, L"Error creating pixel shader", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	context->PSSetShader(pixelShader, nullptr, 0);

	psBlob->Release();
	psBlob = nullptr;
	pixelShader->Release();
	pixelShader = nullptr;







	ID3D11Buffer* vertexBuffer = nullptr;

	// Create an initialize the vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosColor) * _countof(g_Vertices);
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	resourceData.pSysMem = g_Vertices;

	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &resourceData, &vertexBuffer);
	if (FAILED(hr)){
		MessageBox(NULL, L"Error creating vertex buffer", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}





	ID3D11Buffer* indexBuffer = nullptr;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(WORD) * _countof(g_Indicies);
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	resourceData.pSysMem = g_Indicies;

	hr = device->CreateBuffer(&indexBufferDesc, &resourceData, &indexBuffer);
	if (FAILED(hr)){
		MessageBox(NULL, L"Error creating index buffer", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}


	UINT vertexStride = sizeof(VertexPosColor);
	const UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &offset);
	context->IASetInputLayout(inputLayout);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	

	return true;
}

void GraphicsClass::Shutdown(){
	if (context != nullptr){
		context->ClearState();
		context->Release();
		context = nullptr;
	}

	if (renderTargetView != nullptr){
		renderTargetView->Release();
		renderTargetView = nullptr;
	}

	if (swapChain != nullptr){
		swapChain->Release();
		swapChain = nullptr;
	}

	if (depthStencilView != nullptr){
		depthStencilView->Release();
		depthStencilView = nullptr;
	}

	if (rasterState != nullptr){
		rasterState->Release();
		rasterState = nullptr;
	}

	if (depthStencilState != nullptr){
		depthStencilState->Release();
		depthStencilState = nullptr;
	}

	if (backBufferPtr != nullptr){
		backBufferPtr->Release();
		backBufferPtr = nullptr;
	}

	if (depthStencilBuffer != nullptr){
		depthStencilBuffer->Release();
		depthStencilBuffer = nullptr;
	}
	
	if (device != nullptr){
		//ID3D11Debug* debug = 0;
		//pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
		//debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		//debug->Release();

		device->Release();
		device = nullptr;
	}
}

bool GraphicsClass::Render(){
	FLOAT color[4] = { 0.0F, 0.5F, 0.0F, 1.0F };
	// Clear the back buffer.
	context->ClearRenderTargetView(renderTargetView, color);

	// Clear the depth buffer.
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	context->DrawIndexed(_countof(g_Indicies), 0, 0);


	if (VSYNC_ENABLED){
		// Lock to screen refresh rate.
		swapChain->Present(1, 0);
	}else{
		// Present as fast as possible.
		swapChain->Present(0, 0);
	}


	return true;
}

void GraphicsClass::GetNumeratorAndDenominator(UINT& numerator, UINT& denominator){
	HRESULT result;

	UINT screenWidth{ (UINT)GetSystemMetrics(SM_CXSCREEN) };
	UINT screenHeight{ (UINT)GetSystemMetrics(SM_CYSCREEN) };

	IDXGIFactory* factory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)){
		return;
	}

	std::vector <IDXGIAdapter*> adapters;
	GetAdapters(adapters, factory);
	
	for (IDXGIAdapter* adapter : adapters){
		IDXGIOutput* adapterOutput;
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result)){
			continue;
		}

		UINT numModes{ 0 };
		DXGI_FORMAT format{ DXGI_FORMAT_R8G8B8A8_UNORM };

		// Get the number of elements
		result = adapterOutput->GetDisplayModeList(format, 0, &numModes, NULL);
		if (numModes == 0 || FAILED(result)){
			continue;
		}

		DXGI_MODE_DESC* displayModes{ new DXGI_MODE_DESC[numModes] };

		// Get the list
		result = adapterOutput->GetDisplayModeList(format, 0, &numModes, displayModes);
		if (FAILED(result)){
			delete[] displayModes;
			displayModes = nullptr;

			adapterOutput->Release();
			adapterOutput = nullptr;

			continue;
		}

		for (UINT i = 0; i < numModes; i++){
			DXGI_MODE_DESC modeDesc = displayModes[i];
			//char msgbuf[256];
			//sprintf_s(msgbuf, "width: %d, height: %d\n", displayModes[i].Width, displayModes[i].Height);
			//OutputDebugStringA(msgbuf);

			if (modeDesc.Width == screenWidth && modeDesc.Height == screenHeight){
				numerator = displayModes[i].RefreshRate.Numerator;
				denominator = displayModes[i].RefreshRate.Denominator;

				break;
			}
		}

		delete[] displayModes;
		displayModes = nullptr;

		adapterOutput->Release();
		adapterOutput = nullptr;

		if (numerator > 0 && denominator > 0){
			break;
		}
	}

	for (IDXGIAdapter* adapter : adapters){
		// Release the adapter.
		adapter->Release();
		adapter = nullptr;
	}

	adapters.clear();

	// Release the factory.
	factory->Release();
	factory = nullptr;
}

void GraphicsClass::GetAdapters(std::vector <IDXGIAdapter*>& adapters, IDXGIFactory* factory){
	IDXGIAdapter* adapter;
	for (UINT i = 0; factory->EnumAdapters(i, &adapter) == S_OK; i++){
		/*DXGI_ADAPTER_DESC adapterDesc;
		ZeroMemory(&adapterDesc, sizeof(adapterDesc));
		adapter->GetDesc(&adapterDesc);
		OutputDebugStringW(adapterDesc.Description);
		OutputDebugStringW(L"\n");*/

		adapters.push_back(adapter);
	}
}

bool GraphicsClass::CreateDeviceAndSwapChain(UINT& numerator, UINT& denominator, const HWND& hWnd){
	DXGI_SWAP_CHAIN_DESC chainDesc;
	ZeroMemory(&chainDesc, sizeof(chainDesc));
	chainDesc.BufferCount = 1;
	chainDesc.BufferDesc.Width = WIDTH;
	chainDesc.BufferDesc.Height = HEIGHT;
	chainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	chainDesc.BufferDesc.RefreshRate.Numerator = numerator;
	chainDesc.BufferDesc.RefreshRate.Denominator = denominator;

	chainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	chainDesc.OutputWindow = hWnd;
	chainDesc.SampleDesc.Count = 1;
	chainDesc.SampleDesc.Quality = 0;

	if (FULL_SCREEN){
		chainDesc.Windowed = false;
	}
	else{
		chainDesc.Windowed = true;
	}

	chainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	chainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	chainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	chainDesc.Flags = 0;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	HRESULT result;
	result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevels,
		numFeatureLevels,
		D3D11_SDK_VERSION,
		&chainDesc,
		&swapChain,
		&device,
		nullptr,
		&context
		);

	if (FAILED(result)){
		MessageBox(NULL, L"Error creating device and swap chain", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

bool GraphicsClass::CreateBackBufferAndView(){
	HRESULT result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBufferPtr));
	if (FAILED(result)){
		MessageBox(NULL, L"Error getting back buffer", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
	if (FAILED(result)){
		MessageBox(NULL, L"Error creating back buffer view", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

bool GraphicsClass::CreateDepthBufferAndView(){
	D3D11_TEXTURE2D_DESC depthBufferDesc;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = WIDTH;
	depthBufferDesc.Height = HEIGHT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	HRESULT result = device->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer);
	if (FAILED(result)){
		MessageBox(NULL, L"Error creating depth stencil buffer texture", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;


	// Create the depth stencil view.
	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(result)){
		MessageBox(NULL, L"Error creating depth stencil view", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

bool GraphicsClass::CreateDepthStencilState(){
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	HRESULT result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result)){
		MessageBox(NULL, L"Error creating depth stencil state", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

bool GraphicsClass::CreateRasterizerState(){
	D3D11_RASTERIZER_DESC rasterDesc;

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	HRESULT result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result)){
		MessageBox(NULL, L"Error creating rasterizer state", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}