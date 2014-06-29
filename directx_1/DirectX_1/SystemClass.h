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
	bool InitWindow(HINSTANCE hInstance, int nCmdShow);
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static SystemClass* ApplicationHandle;

private:
	HWND hWnd;
	HINSTANCE hInstance;

	InputClass* input;
	GraphicsClass* graphics;
};