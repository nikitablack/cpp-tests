#pragma once

#include <Windows.h>

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void KeyDown(UINT);
	void KeyUp(UINT);

	bool IsKeyDown(UINT);

private:
	bool m_keys[256];
};