#pragma once

#include <functional>
#include <string>

#include <Windows.h>

class Window
{
public:
	Window(LONG const width, LONG const height, std::function<void(WPARAM)> const onKeyPress);
	HWND getHandle() const;
	POINT getSize() const;
	void setTitle(std::string const & str) const;

private:
	static LRESULT CALLBACK WndProc(HWND const hWnd, UINT const message, WPARAM const wParam, LPARAM const lParam);

private:
	HWND _hWnd;
	std::function<void(WPARAM)> _onKeyPress;
};