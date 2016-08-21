#include "ShapesOOP.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	const LONG width{ 800 };
	const LONG height{ 600 };
	const UINT bufferCount{ 3 };

	shared_ptr<ShapesOOP> shapesDemo;

	try
	{
		shapesDemo = make_shared<ShapesOOP>(1);
	}
	catch (runtime_error& err)
	{
		MessageBox(nullptr, err.what(), "Error", MB_OK);
		return 0;
	}

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		BOOL r{ PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) };
		if (r == 0)
		{
			try
			{
				shapesDemo->update(1.0f);
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