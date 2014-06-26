#include "InputClass.h"

InputClass::InputClass(){
	for (int i = 0; i<256; i++)
	{
		m_keys[i] = false;
	}
}


InputClass::InputClass(const InputClass& other){
}


InputClass::~InputClass(){
}

void InputClass::KeyDown(UINT input){
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(UINT input){
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(UINT key){
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}