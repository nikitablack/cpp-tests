#include "Window.h"
#include <stdexcept>

using namespace std;

Window::Window(LONG width, LONG height, LPCSTR title)
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

	hWnd = CreateWindowEx(0, "WindowClass", title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, nullptr, this);
	if (!hWnd)
	{
		throw(runtime_error{ "Error creating window." });
	}

	ShowWindow(hWnd, SW_SHOW);
}

HWND Window::getHandle()
{
	return hWnd;
}

POINT Window::getMousePosition()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(hWnd, &p);
	return p;
}

POINT Window::getSize()
{
	RECT rect;
	if (!GetClientRect(hWnd, &rect))
	{
		throw(runtime_error{ "Error getting window size." });
	}

	return{ rect.right - rect.left, rect.bottom - rect.top };
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Window* window{ reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)) };

	switch (message)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT pCreateStruct{ reinterpret_cast<LPCREATESTRUCT>(lParam) };
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (window->onKeyPress != nullptr)
			(*window->onKeyPress)(wParam);

		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Window::addKeyPressCallback(shared_ptr<function<void(WPARAM)>> onKeyPress)
{
	this->onKeyPress = onKeyPress;
}

void Window::setTitle(const string & str)
{
	SetWindowText(hWnd, str.data());
}