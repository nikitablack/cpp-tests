#include "ShapesOOP.h"
#include <chrono>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	shared_ptr<ShapesOOP> shapesDemo;

	try
	{
		shapesDemo = make_shared<ShapesOOP>(100, 1366.0f, 768.0f);
	}
	catch (runtime_error& err)
	{
		MessageBox(nullptr, err.what(), "Error", MB_OK);
		return 0;
	}

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	float dt{ 0.0f };

	while (msg.message != WM_QUIT)
	{
		BOOL r{ PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) };
		if (r == 0)
		{
			try
			{
				auto start = chrono::high_resolution_clock::now();
				shapesDemo->update(dt);
				auto end = chrono::high_resolution_clock::now();
				auto time = chrono::duration_cast<chrono::milliseconds>(end - start);
				dt = time.count() / 1000.0f;
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