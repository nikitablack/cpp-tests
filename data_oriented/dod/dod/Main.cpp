#include <chrono>
#include <stdexcept>
#include <Windows.h>

#include "ShapesApp.h"
#include "Window.h"

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	ShapesApp & app{ ShapesApp::getInstance(50, 800.0f, 600.0f) };

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	float dt{ 0.0f };

	using namespace chrono;
	while (msg.message != WM_QUIT)
	{
		BOOL const r{ PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) };
		if (r == 0)
		{
			try
			{
				auto const start{ high_resolution_clock::now() };
				app.update(dt);
				duration<float> const time{ high_resolution_clock::now() - start };
				dt = time.count();
			}
			catch (runtime_error& err)
			{
				MessageBox(nullptr, err.what(), "Error", MB_OK);
				return 0;
			}
		}
		else
		{
			DispatchMessage(&msg);
		}
	}

	return 0;
}