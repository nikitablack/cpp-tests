#pragma once

#include <functional>
#include <string>
#include <Windows.h>

#include "Math.h"

class Window
{
public:
	enum Key
	{
		PLUS,
		MINUS
	};

public:
	Window(float const x, float const y, float const width, float const height, std::string const & title);
	HWND getHandle() const;
	math::Vec2 getSize() const;
	void setTitle(std::string const & str) const;
	void setKeyPressCallback(std::function<void(Key)> const onKeyPress);

private:
	static LRESULT CALLBACK WndProc(HWND const hWnd, UINT const message, WPARAM const wParam, LPARAM const lParam);

private:
	HWND _hWnd;
	std::function<void(Key)> _onKeyPress;
};