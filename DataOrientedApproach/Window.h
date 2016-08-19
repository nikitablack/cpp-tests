#pragma once

#include <Windows.h>

class Window
{
public:
	Window(LONG width, LONG height, LPCSTR title);
	HWND getHandle();
	POINT getMousePosition();
	POINT getSize();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND hWnd;
};