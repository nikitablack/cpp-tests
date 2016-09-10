#pragma once

#include <Windows.h>
#include <memory>
#include <functional>

class Window
{
public:
	Window(LONG width, LONG height, LPCSTR title);
	HWND getHandle();
	POINT getMousePosition();
	POINT getSize();
	void addKeyPressCallback(std::shared_ptr<std::function<void(WPARAM)>> onKeyPress);
	void setTitle(const std::string& str);

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND hWnd;
	std::shared_ptr<std::function<void(WPARAM)>> onKeyPress;
};