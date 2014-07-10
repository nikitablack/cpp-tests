// System includes
#include <Windows.h>
#include <algorithm>
#include <memory>

// DirectX includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// Link library dependencies
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")

// STL includes
//#include <iostream>
#include <string>

// Safely release a COM object.
template<typename T>
inline void SafeRelease(T& ptr)
{
	if (ptr != nullptr){
		ptr->Release();
		ptr = nullptr;
	}
};

struct release_COM
{
	template<typename T>
	void operator()(T* ptr) const
	{
		ptr->Release();
	}
};