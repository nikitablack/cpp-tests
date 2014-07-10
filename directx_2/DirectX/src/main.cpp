#include "DirectXTemplatePCH.h"

using namespace DirectX;

const LONG g_WindowWidth{ 800 };
const LONG g_WindowHeight{ 600 };
LPCWSTR g_WindowClassName{ L"DirectXWindowClass" };
LPCWSTR g_WindowName{ L"DirectX Test" };
HWND g_WindowHandle{ nullptr };
const bool g_EnableVSync{ true };

// Direct3D device and swap chain.
ID3D11Device* g_Device{ nullptr };
ID3D11DeviceContext* g_Context{ nullptr };
IDXGISwapChain* g_SwapChain{ nullptr };

// Render target view for the back buffer of the swap chain.
ID3D11RenderTargetView* g_RenderTargetView{ nullptr };
// Depth/stencil view for use as a depth buffer.
ID3D11DepthStencilView* g_DepthStencilView{ nullptr };
// A texture to associate to the depth stencil view.
ID3D11Texture2D* g_DepthStencilBuffer{ nullptr };

// Define the functionality of the depth/stencil stages.
ID3D11DepthStencilState* g_DepthStencilState{ nullptr };
// Define the functionality of the rasterizer stage.
ID3D11RasterizerState* g_RasterizerState{ nullptr };
D3D11_VIEWPORT g_Viewport{};

// Vertex buffer data
ID3D11InputLayout* g_InputLayout{ nullptr };
ID3D11Buffer* g_VertexBuffer{ nullptr };
ID3D11Buffer* g_IndexBuffer{ nullptr };

// Shader data
ID3D11VertexShader* g_VertexShader{ nullptr };
ID3D11PixelShader* g_PixelShader{ nullptr };

// Shader resources
/*enum class ConstanBuffer
{
	CB_Appliation,
	CB_Frame,
	CB_Object
};*/
const unsigned long long g_NumConstantBuffers{ 3 };

ID3D11Buffer* g_ConstantBuffers[g_NumConstantBuffers]{nullptr};

// Demo parameters
XMMATRIX g_WorldMatrix;
XMMATRIX g_ViewMatrix;
XMMATRIX g_ProjectionMatrix;

// Vertex data for a colored cube.
struct VertexPosColor
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

VertexPosColor g_Vertices[8] =
{
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
};

WORD g_Indicies[36] =
{
	0, 1, 2,
	0, 2, 3,
	4, 6, 5,
	4, 7, 6,
	4, 5, 1,
	4, 1, 0,
	3, 2, 6,
	3, 6, 7,
	1, 5, 6,
	1, 6, 2,
	4, 0, 3,
	4, 3, 7
};

bool g_IsFullscreen = false;

// Forward declarations.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int InitApplication(HINSTANCE hInstance, int cmdShow);
int Run();

HRESULT InitDirectX(HINSTANCE hInstance);

//template< class ShaderClass >
//ShaderClass* LoadShader(const std::wstring& fileName, const std::string& entryPoint, const std::string& profile);

bool LoadContent();
void UnloadContent();

void Update(float deltaTime);
void Render();
void Cleanup();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	// Check for DirectX Math library support.
	if (!XMVerifyCPUSupport()){
		MessageBox(nullptr, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
		return -1;
	}

	if (InitApplication(hInstance, cmdShow) != 0){
		MessageBox(nullptr, TEXT("Failed to create applicaiton window."), TEXT("Error"), MB_OK);
		return -1;
	}

	HRESULT result = InitDirectX(hInstance);
	if (FAILED(result)){
		MessageBox(nullptr, TEXT("Failed to init direcX."), TEXT("Error"), MB_OK);
		return -1;
	}

	int returnCode = Run();

	SafeRelease(g_Device);
	SafeRelease(g_Context);
	SafeRelease(g_SwapChain);
	SafeRelease(g_RenderTargetView);
	SafeRelease(g_DepthStencilBuffer);
	SafeRelease(g_DepthStencilView);
	SafeRelease(g_DepthStencilState);
	SafeRelease(g_RasterizerState);
	SafeRelease(g_VertexShader);
	SafeRelease(g_VertexBuffer);
	SafeRelease(g_IndexBuffer);
	SafeRelease(g_InputLayout);
	SafeRelease(g_PixelShader);

	for (int i = 0; i < g_NumConstantBuffers; i++){
		SafeRelease(g_ConstantBuffers[i]);
	}

	return returnCode;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//PAINTSTRUCT paintStruct;
	//HDC hDC;

	switch (message){
		/*case WM_PAINT:
			hDC = BeginPaint(hwnd, &paintStruct);
			EndPaint(hwnd, &paintStruct);
			break;*/
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_LBUTTONDOWN:
			if (g_IsFullscreen)
			{
				LONG dwStyle{ GetWindowLong(hwnd, GWL_STYLE) };
				LONG dwAdd{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX };
				LONG dwNewStyle{ dwStyle | dwAdd };
				SetWindowLongPtr(hwnd, GWL_STYLE, dwNewStyle);

				RECT windowRect{ 0, 0, g_WindowWidth, g_WindowHeight };
				AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

				LONG clientWidth{ windowRect.right - windowRect.left };
				LONG clientHeight{ windowRect.bottom - windowRect.top };

				int posX{ (GetSystemMetrics(SM_CXSCREEN) - clientWidth) >> 1 };
				int posY{ (GetSystemMetrics(SM_CYSCREEN) - clientHeight) >> 1 };

				SetWindowPos(hwnd, HWND_TOP, posX, posY, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_FRAMECHANGED);
			}
			else{
				LONG dwStyle{ GetWindowLong(hwnd, GWL_STYLE) };
				LONG dwRemove{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX };
				LONG dwNewStyle{ dwStyle & ~dwRemove };
				SetWindowLongPtr(hwnd, GWL_STYLE, dwNewStyle);

				SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
			}
			g_IsFullscreen = !g_IsFullscreen;

			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

int InitApplication(HINSTANCE hInstance, int cmdShow)
{
	WNDCLASSEX wndClass{};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = &WndProc;
	wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = g_WindowClassName;

	if (!RegisterClassEx(&wndClass)){
		return -1;
	}

	RECT windowRect{ 0, 0, g_WindowWidth, g_WindowHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	LONG clientWidth{ windowRect.right - windowRect.left };
	LONG clientHeight{ windowRect.bottom - windowRect.top };

	int posX{ (GetSystemMetrics(SM_CXSCREEN) - clientWidth) >> 1 };
	int posY{ (GetSystemMetrics(SM_CYSCREEN) - clientHeight) >> 1 };

	g_WindowHandle = CreateWindow(g_WindowClassName, g_WindowName,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);

	if (!g_WindowHandle){
		return -1;
	}

	ShowWindow(g_WindowHandle, cmdShow);
	UpdateWindow(g_WindowHandle);

	return 0;
}

int Run(){
	MSG msg = { 0 };

	static DWORD previousTime{ timeGetTime() };
	static const float targetFramerate{ 60.0f };
	static const float maxTimeStep{ 1.0f / targetFramerate };

	while (msg.message != WM_QUIT){
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else{
			DWORD currentTime{ timeGetTime() };
			float deltaTime{ (currentTime - previousTime) / 1000.0f };
			previousTime = currentTime;

			// Cap the delta time to the max time step (useful if your 
			// debugging and you don't want the deltaTime value to explode.
			deltaTime = std::min<float>(deltaTime, maxTimeStep);

			Update( deltaTime );
			Render();
		}
	}

	return static_cast<int>(msg.wParam);
}

HRESULT InitDirectX(HINSTANCE hInstance){
	// A window handle must have been created already.
	assert(g_WindowHandle != 0);

	RECT clientRect{};
	GetClientRect(g_WindowHandle, &clientRect);

	// Compute the exact client dimensions. This will be used
	// to initialize the render targets for our swap chain.
	LONG clientWidth{ clientRect.right - clientRect.left };
	LONG clientHeight{ clientRect.bottom - clientRect.top };

	// Describe swap chain
	DXGI_RATIONAL RefreshRate{};
	ZeroMemory(&RefreshRate, sizeof(DXGI_RATIONAL));
	RefreshRate.Numerator = 60;
	RefreshRate.Denominator = 1;

	DXGI_MODE_DESC BufferDesc{};
	ZeroMemory(&BufferDesc, sizeof(DXGI_MODE_DESC));
	BufferDesc.Width = clientWidth;;
	BufferDesc.Height = clientHeight;
	BufferDesc.RefreshRate = RefreshRate;
	BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// TODO request supported aliasing ID3D11Device::CheckMultisampleQualityLevels
	DXGI_SAMPLE_DESC SampleDesc{};
	SampleDesc.Count = 1;
	SampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC SwapChainDesc{};
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	SwapChainDesc.BufferDesc = BufferDesc;
	SwapChainDesc.SampleDesc = SampleDesc;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 2;
	SwapChainDesc.OutputWindow = g_WindowHandle;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags = 0;

	// Create device, context and swap chain
	UINT createDeviceFlags = 0;
#if _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// These are the feature levels that we will accept.
	D3D_FEATURE_LEVEL featureLevels[]{D3D_FEATURE_LEVEL_11_0};

	// This will be the feature level that 
	// is used to create our device and swap chain.
	//D3D_FEATURE_LEVEL featureLevel;

	// TODO
	// define adapter
	HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
		D3D11_SDK_VERSION, &SwapChainDesc, &g_SwapChain, &g_Device, nullptr,
		&g_Context);

	if (FAILED(result)){
		return result;
	}

	// Create render target view
	std::unique_ptr<ID3D11Texture2D, release_COM> backBufferUniquePtr{nullptr};
	ID3D11Texture2D* backBuffer{ nullptr };
	result = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(result)){
		return result;
	}

	backBufferUniquePtr.reset(backBuffer);

	result = g_Device->CreateRenderTargetView(backBufferUniquePtr.get(), nullptr, &g_RenderTargetView);
	if (FAILED(result)){
		return result;
	}

	// Create the depth stencil buffer
	D3D11_TEXTURE2D_DESC DepthStencilBufferDesc{};
	ZeroMemory(&DepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	DepthStencilBufferDesc.Width = clientWidth;
	DepthStencilBufferDesc.Height = clientHeight;
	DepthStencilBufferDesc.MipLevels = 1;
	DepthStencilBufferDesc.ArraySize = 1;
	DepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilBufferDesc.SampleDesc = SampleDesc;
	DepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthStencilBufferDesc.CPUAccessFlags = 0;
	DepthStencilBufferDesc.MiscFlags = 0;

	/*D3D11_SUBRESOURCE_DATA DepthStencilSubresource;
	DepthStencilSubresource.pSysMem;
	DepthStencilSubresource.SysMemPitch;
	DepthStencilSubresource.SysMemSlicePitch*/

	result = g_Device->CreateTexture2D(&DepthStencilBufferDesc, nullptr, &g_DepthStencilBuffer);
	if (FAILED(result)){
		return result;
	}

	// Create the depth stencil view
	/*D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc{ };
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Flags = 0;
	DepthStencilViewDesc.Texture2D;*/

	result = g_Device->CreateDepthStencilView(g_DepthStencilBuffer, nullptr, &g_DepthStencilView);
	if (FAILED(result)){
		return result;
	}

	// Setup depth/stencil state.
	D3D11_DEPTH_STENCIL_DESC DepthStencilStateDesc{};
	ZeroMemory(&DepthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	DepthStencilStateDesc.DepthEnable = TRUE;
	DepthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	DepthStencilStateDesc.StencilEnable = FALSE;
	DepthStencilStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DepthStencilStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	/*DepthStencilStateDesc.FrontFace;
	DepthStencilStateDesc.BackFace*/

	result = g_Device->CreateDepthStencilState(&DepthStencilStateDesc, &g_DepthStencilState);
	if (FAILED(result)){
		return result;
	}

	// Setup rasterizer state.
	D3D11_RASTERIZER_DESC RasterizerDesc{};
	ZeroMemory(&RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;//D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_NONE;// D3D11_CULL_BACK;
	RasterizerDesc.FrontCounterClockwise = FALSE;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = TRUE;
	RasterizerDesc.ScissorEnable = FALSE;
	RasterizerDesc.MultisampleEnable = FALSE;
	RasterizerDesc.AntialiasedLineEnable = FALSE;

	// Create the rasterizer state object.
	result = g_Device->CreateRasterizerState(&RasterizerDesc, &g_RasterizerState);
	if (FAILED(result)){
		return result;
	}

	g_Viewport.Width = static_cast<float>(clientWidth);
	g_Viewport.Height = static_cast<float>(clientHeight);
	g_Viewport.TopLeftX = 0.0f;
	g_Viewport.TopLeftY = 0.0f;
	g_Viewport.MinDepth = 0.0f;
	g_Viewport.MaxDepth = 1.0f;

	// Vertex buffer
	D3D11_BUFFER_DESC VertexBufferDesc{};
	ZeroMemory(&VertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	VertexBufferDesc.ByteWidth = sizeof(VertexPosColor) * _countof(g_Vertices);
	VertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;// D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VerticesResourceData{};
	ZeroMemory(&VerticesResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	VerticesResourceData.pSysMem = g_Vertices;
	VerticesResourceData.SysMemPitch = 0;
	VerticesResourceData.SysMemSlicePitch = 0;

	result = g_Device->CreateBuffer(&VertexBufferDesc, &VerticesResourceData, &g_VertexBuffer);
	if (FAILED(result)){
		return result;
	}

	// Index buffer
	D3D11_BUFFER_DESC IndexBufferDesc{};
	ZeroMemory(&IndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	IndexBufferDesc.ByteWidth = sizeof(VertexPosColor) * _countof(g_Indicies);
	IndexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;// D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA IndicesResourceData{};
	ZeroMemory(&IndicesResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	IndicesResourceData.pSysMem = g_Indicies;
	IndicesResourceData.SysMemPitch = 0;
	IndicesResourceData.SysMemSlicePitch = 0;

	result = g_Device->CreateBuffer(&IndexBufferDesc, &IndicesResourceData, &g_IndexBuffer);
	if (FAILED(result)){
		return result;
	}

	// Constant buffers
	D3D11_BUFFER_DESC ConstantBufferDesc{};
	ZeroMemory(&ConstantBufferDesc, sizeof(D3D11_BUFFER_DESC));
	ConstantBufferDesc.ByteWidth = sizeof(XMMATRIX);
	ConstantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.CPUAccessFlags = 0;
	ConstantBufferDesc.MiscFlags = 0;
	ConstantBufferDesc.StructureByteStride = 0;

	result = g_Device->CreateBuffer(&ConstantBufferDesc, nullptr, &g_ConstantBuffers[0]);
	if (FAILED(result)){
		return result;
	}

	result = g_Device->CreateBuffer(&ConstantBufferDesc, nullptr, &g_ConstantBuffers[1]);
	if (FAILED(result)){
		return result;
	}

	result = g_Device->CreateBuffer(&ConstantBufferDesc, nullptr, &g_ConstantBuffers[2]);
	if (FAILED(result)){
		return result;
	}

	// vertex shader
	std::unique_ptr<ID3DBlob, release_COM> vsBlobUniquePtr{ nullptr };
	ID3DBlob* vsBlob{ nullptr };
	result = D3DReadFileToBlob(L"VertexShader.cso", &vsBlob);
	if (FAILED(result)){
		return result;
	}
	vsBlobUniquePtr.reset(vsBlob);

	result = g_Device->CreateVertexShader(vsBlobUniquePtr.get()->GetBufferPointer(), vsBlobUniquePtr.get()->GetBufferSize(), nullptr, &g_VertexShader);
	if (FAILED(result)){
		return result;
	}

	D3D11_INPUT_ELEMENT_DESC VertexPositionDesc{};
	VertexPositionDesc.SemanticName = "POSITION";
	VertexPositionDesc.SemanticIndex = 0;
	VertexPositionDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	VertexPositionDesc.InputSlot = 0;
	VertexPositionDesc.AlignedByteOffset = offsetof(VertexPosColor, Position);
	VertexPositionDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	VertexPositionDesc.InstanceDataStepRate = 0;

	D3D11_INPUT_ELEMENT_DESC VertexColorDesc{};
	VertexColorDesc.SemanticName = "COLOR";
	VertexColorDesc.SemanticIndex = 0;
	VertexColorDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	VertexColorDesc.InputSlot = 0;
	VertexColorDesc.AlignedByteOffset = offsetof(VertexPosColor, Color);
	VertexColorDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	VertexColorDesc.InstanceDataStepRate = 0;

	D3D11_INPUT_ELEMENT_DESC VertexLayoutDesc[] = {VertexPositionDesc, VertexColorDesc};

	result = g_Device->CreateInputLayout(VertexLayoutDesc, _countof(VertexLayoutDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &g_InputLayout);
	if (FAILED(result)){
		MessageBox(NULL, L"Error creating input layout", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// pixel shader
	std::unique_ptr<ID3DBlob, release_COM> psBlobUniquePtr{ nullptr };
	ID3DBlob* psBlob{ nullptr };
	result = D3DReadFileToBlob(L"PixelShader.cso", &psBlob);
	if (FAILED(result)){
		return result;
	}
	psBlobUniquePtr.reset(psBlob);

	result = g_Device->CreatePixelShader(psBlobUniquePtr.get()->GetBufferPointer(), psBlobUniquePtr.get()->GetBufferSize(), nullptr, &g_PixelShader);
	if (FAILED(result)){
		return result;
	}

	// projection matrix
	g_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), static_cast<float>(clientWidth) / static_cast<float>(clientHeight), 0.1f, 100.0f);

	g_Context->UpdateSubresource(g_ConstantBuffers[0], 0, nullptr, &g_ProjectionMatrix, 0, 0);

	return S_OK;
};

void Update(float deltaTime){
	XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
	XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
	XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
	g_ViewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
	g_Context->UpdateSubresource(g_ConstantBuffers[1], 0, nullptr, &g_ViewMatrix, 0, 0);

	static float angle = 0.0f;
	angle += 45.0f * deltaTime;
	XMVECTOR rotationAxis = XMVectorSet(0, 1, 0, 0);

	g_WorldMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
	g_Context->UpdateSubresource(g_ConstantBuffers[2], 0, nullptr, &g_WorldMatrix, 0, 0);
}

void Render(){
	assert(g_Device);
	assert(g_Context);
	
	static FLOAT color[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
	g_Context->ClearRenderTargetView(g_RenderTargetView, color);
	g_Context->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	const UINT vertexStride{ sizeof(VertexPosColor) };
	const UINT offset = 0;

	g_Context->IASetVertexBuffers(0, 1, &g_VertexBuffer, &vertexStride, &offset);
	g_Context->IASetInputLayout(g_InputLayout);
	g_Context->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_Context->VSSetShader(g_VertexShader, nullptr, 0);
	g_Context->VSSetConstantBuffers(0, g_NumConstantBuffers, g_ConstantBuffers);

	g_Context->RSSetState(g_RasterizerState);
	g_Context->RSSetViewports(1, &g_Viewport);

	g_Context->PSSetShader(g_PixelShader, nullptr, 0);

	g_Context->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_Context->OMSetDepthStencilState(g_DepthStencilState, 1);

	//g_Context->DrawIndexed(_countof(g_Indicies), 0, 0);

	if (g_EnableVSync){
		g_SwapChain->Present(1, 0);
	}else{
		g_SwapChain->Present(0, 0);
	}
}