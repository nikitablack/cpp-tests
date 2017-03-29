#include <optional>
#include <stdexcept>

#include "Window.h"

using namespace math;
using namespace std;

Window::Window(float const x, float const y, float const width, float const height, std::string const & title)
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

	RECT rect{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	_hWnd = CreateWindowEx(0, "WindowClass", title.c_str(), WS_OVERLAPPEDWINDOW, static_cast<LONG>(x), static_cast<LONG>(y), rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, nullptr, this);
	if (!_hWnd)
	{
		throw(runtime_error{ "Error creating window." });
	}

	ShowWindow(_hWnd, SW_SHOW);
}

HWND Window::getHandle() const
{
	return _hWnd;
}

Vec2 Window::getSize() const
{
	RECT rect;
	if (!GetClientRect(_hWnd, &rect))
	{
		throw(runtime_error{ "Error getting window size." });
	}

	return{ static_cast<float>(rect.right - rect.left), static_cast<float>(rect.bottom - rect.top) };
}

void Window::setTitle(string const & str) const
{
	SetWindowText(_hWnd, str.c_str());
}

void Window::setKeyPressCallback(function<void(Key)> const onKeyPress)
{
	_onKeyPress = onKeyPress;
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
	{
		optional<Key> key;
		switch (wParam)
		{
		case 107:
			key = Key::PLUS;
			break;
		case 109:
			key = Key::MINUS;
			break;
		}

		if (key.has_value() && window->_onKeyPress != nullptr)
			(window->_onKeyPress)(key.value());

		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}