#include "SystemClass.h"
#include <stdio.h>

SystemClass *SystemClass::ApplicationHandle = nullptr;

SystemClass::SystemClass(){
	input = nullptr;
	graphics = nullptr;

	applicationName = L"Engine";
}

SystemClass::SystemClass(const SystemClass& other){
}

SystemClass::~SystemClass(){
}

bool SystemClass::Initialize(HINSTANCE hInstance, int nCmdShow){
	SystemClass::ApplicationHandle = this;

	UINT screenWidth{ 0 };
	UINT screenHeight{ 0 };

	if (!InitWindow(hInstance, nCmdShow, screenWidth, screenHeight)){
		return false;
	}

	input = new InputClass;
	if (!input){
		MessageBox(NULL, L"Error creating input", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	graphics = new GraphicsClass;
	if (graphics == nullptr){
		MessageBox(NULL, L"Error creating graphics", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!graphics->Initialize(screenWidth, screenHeight, hWnd)){
		return false;
	}

	return true;
}

void SystemClass::Shutdown(){
	// Remove the window.
	DestroyWindow(hWnd);
	hWnd = nullptr;

	// Remove the application instance.
	UnregisterClass(applicationName, hInstance);
	hInstance = nullptr;

	if (input != nullptr)
	{
		delete input;
		input = nullptr;
	}

	if (graphics != nullptr){
		graphics->Shutdown();
		delete graphics;
		graphics = nullptr;
	}
}

void SystemClass::Run(){
	MSG msg = { 0 };
	while (WM_QUIT != msg.message){
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (input->IsKeyDown(VK_ESCAPE)){
			return;
		} else{
			if (!graphics->Render()){
				return;
			}
		}
	}
}

bool SystemClass::InitWindow(HINSTANCE hInstance, int nCmdShow, UINT& screenWidth, UINT& screenHeight){
	this->hInstance = hInstance;

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &SystemClass::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(nullptr, IDI_SHIELD);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = applicationName;
	wcex.hIconSm = wcex.hIcon;

	DWORD dwLastError = GetLastError();

	if (!RegisterClassEx(&wcex)){
		MessageBox(NULL, L"Error registering window class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	//SM_CMONITORS
	screenWidth = (UINT)GetSystemMetrics(SM_CXSCREEN);
	screenHeight = (UINT)GetSystemMetrics(SM_CYSCREEN);

	// Create window
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow(applicationName, L"Direct3D 11", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);

	if (!hWnd){
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hWnd, nCmdShow);

	return true;
}

LRESULT CALLBACK SystemClass::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_KEYDOWN:
	{
		SystemClass::ApplicationHandle->input->KeyDown((UINT)wParam);
		return 0;
	}

	case WM_KEYUP:
	{
		SystemClass::ApplicationHandle->input->KeyUp((UINT)wParam);
		return 0;
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}