#include "GraphicsClass.h"

GraphicsClass::GraphicsClass(){
	pd3dDevice = nullptr;
	pImmediateContext = nullptr;
	pSwapChain = nullptr;
	pRenderTargetView = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other){
}

GraphicsClass::~GraphicsClass(){
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hWnd){
	HRESULT result;
	
	
	
	//unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	IDXGIFactory* factory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)){
		MessageBox(NULL, L"Error creating DXGIfactory", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	IDXGIAdapter* adapter;
	std::vector <IDXGIAdapter*> adapters;
	for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++){
		DXGI_ADAPTER_DESC ad;
		ZeroMemory(&ad, sizeof(ad));
		adapter->GetDesc(&ad);
		OutputDebugStringW(ad.Description);
		OutputDebugStringW(L"\n");
		adapters.push_back(adapter);
	}

	IDXGIOutput* adapterOutput;
	for (IDXGIAdapter* adapter : adapters){
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result)){
			continue;
		}

		UINT numModes = 0;
		DXGI_MODE_DESC* displayModes = nullptr;
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Get the number of elements
		result = adapterOutput->GetDisplayModeList(format, 0, &numModes, NULL);
		if (numModes == 0 || FAILED(result)){
			continue;
		}

		displayModes = new DXGI_MODE_DESC[numModes];

		// Get the list
		result = adapterOutput->GetDisplayModeList(format, 0, &numModes, displayModes);
		if (FAILED(result)){
			continue;
		}

		UINT numerator{ 0 };
		UINT denominator{ 0 };
		for (int i = 0; i < numModes; i++)
		{
			//char msgbuf[256];
			//sprintf_s(msgbuf, "width: %d, height: %d\n", displayModes[i].Width, displayModes[i].Height);
			//OutputDebugStringA(msgbuf);
			
			if (displayModes[i].Width == (unsigned int)screenWidth)
			{
				if (displayModes[i].Height == (unsigned int)screenHeight)
				{
					numerator = displayModes[i].RefreshRate.Numerator;
					denominator = displayModes[i].RefreshRate.Denominator;

					break;
				}
			}
		}
	}
	
	factory->Release();










	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevels,
		numFeatureLevels,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pd3dDevice,
		&featureLevel,
		&pImmediateContext
	);

	return true;
	
	/*if (hr == E_INVALIDARG)
	{
		// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
		hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
	}*/

	if (FAILED(hr)){
		MessageBox(NULL, L"Error creating device", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Obtain the Direct3D 11 versions if available
	/*hr = pd3dDevice->QueryInterface(__uuidof(ID3D11Device), reinterpret_cast<void**>(&pd3dDevice));
	if (SUCCEEDED(hr)){
		(void)pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext), reinterpret_cast<void**>(&pImmediateContext));
	}else{
		MessageBox(NULL, L"Error creating graphics", L"Error", MB_OK | MB_ICONERROR);
	}*/

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr)){
		MessageBox(NULL, L"Error getting buffer", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr)){
		MessageBox(NULL, L"Error creating render target view", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pImmediateContext->RSSetViewports(1, &vp);

	return true;
}

void GraphicsClass::Shutdown(){
	if (pImmediateContext != nullptr){
		pImmediateContext->ClearState();
		pImmediateContext->Release();
		pImmediateContext = nullptr;
	}

	if (pRenderTargetView != nullptr){
		pRenderTargetView->Release();
		pRenderTargetView = nullptr;
	}

	if (pSwapChain != nullptr){
		pSwapChain->Release();
		pSwapChain = nullptr;
	}
	
	if (pd3dDevice != nullptr){
		//ID3D11Debug* debug = 0;
		//pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
		//debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		//debug->Release();

		pd3dDevice->Release();
		pd3dDevice = nullptr;
	}
}

bool GraphicsClass::Render(){

	return true;
}