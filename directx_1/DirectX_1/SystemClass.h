#pragma once

#include <Windows.h>
#include "InputClass.h"
#include "GraphicsClass.h"

class SystemClass{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize(HINSTANCE hInstance, int nCmdShow);
	void Shutdown();
	void Run();
private:
	bool InitWindow(HINSTANCE hInstance, int nCmdShow, UINT& screenWidth, UINT& screenHeight);
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static SystemClass* ApplicationHandle;

private:
	LPCWSTR applicationName;
	HWND hWnd;
	HINSTANCE hInstance;

	InputClass* input;
	GraphicsClass* graphics;
};