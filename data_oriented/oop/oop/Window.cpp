#include <stdexcept>

#include "Window.h"

using namespace std;

Window::Window(LONG const width, LONG const height, function<void(WPARAM)> const onKeyPress)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = (HMODULE)GetModuleHandle(0);
	wcex.hIcon = LoadIcon(NULL, IDI_SHIELD);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = "WindowClass";
	wcex.hIconSm = LoadIcon(NULL, IDI_WARNING);

	if (RegisterClassEx(&wcex) == 0)
	{
		throw(runtime_error{ "Error registering window." });
	}

	RECT rect{ 0, 0, width, height };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	_hWnd = CreateWindowEx(0, "WindowClass", "", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, nullptr, this);
	if (!_hWnd)
	{
		throw(runtime_error{ "Error creating window." });
	}

	ShowWindow(_hWnd, SW_SHOW);

	_onKeyPress = onKeyPress;
}

HWND Window::getHandle() const
{
	return _hWnd;
}

POINT Window::getSize() const
{
	RECT rect;
	if (!GetClientRect(_hWnd, &rect))
	{
		throw(runtime_error{ "Error getting window size." });
	}

	return{ rect.right - rect.left, rect.bottom - rect.top };
}

void Window::setTitle(string const & str) const
{
	SetWindowText(_hWnd, str.data());
}

LRESULT CALLBACK Window::WndProc(HWND const hWnd, UINT const message, WPARAM const wParam, LPARAM const lParam)
{
	Window * const window{ reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)) };

	switch (message)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT const pCreateStruct{ reinterpret_cast<LPCREATESTRUCT>(lParam) };
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (window->_onKeyPress != nullptr)
			(window->_onKeyPress)(wParam);

		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}