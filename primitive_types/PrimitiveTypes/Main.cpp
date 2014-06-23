#include <iostream>
#include <limits>

using namespace std;

int main(){
	// char
	char char1{ 50 };
	cout << char1 << endl;

	char char2{ 'a' };
	cout << char2 << endl;

	char char3{ '\xFF' };
	cout << char3 << endl;

	cout << "size of char: " << sizeof(char) << endl;

	std::cout << "min char value: " << (int)std::numeric_limits<char>::min() << std::endl;
	std::cout << "max char value: " << (int)std::numeric_limits<char>::max() << std::endl;

	// signed char

	std::cout << "min signed char value: " << (int)std::numeric_limits<signed char>::min() << std::endl;
	std::cout << "max signed char value: " << (int)std::numeric_limits<signed char>::max() << std::endl;

	// unsigned char
	std::cout << "min unsigned char value: " << (int)std::numeric_limits<unsigned char>::min() << std::endl;
	std::cout << "max unsigned char value: " << (int)std::numeric_limits<unsigned char>::max() << std::endl;

	// wchar_t
	wchar_t char4{ L'\xFFFF' };
	cout << char4 << endl;

	cout << "size of wchar_t: " << sizeof(wchar_t) << endl;

	std::cout << "min wchar_t value: " << (int)std::numeric_limits<wchar_t>::min() << std::endl;
	std::cout << "max wchar_t value: " << (int)std::numeric_limits<wchar_t>::max() << std::endl;

	// char16_t
	cout << "size of char16_t: " << sizeof(char16_t) << endl;

	std::cout << "min char16_t value: " << (int)std::numeric_limits<char16_t>::min() << std::endl;
	std::cout << "max char16_t value: " << (int)std::numeric_limits<char16_t>::max() << std::endl;

	// char32_t
	cout << "size of char32_t: " << sizeof(char32_t) << endl;

	std::cout << "min char32_t value: " << (int)std::numeric_limits<char32_t>::min() << std::endl;
	std::cout << "max char32_t value: " << (int)std::numeric_limits<char32_t>::max() << std::endl; // -1 wtf?????

	int a;
	cin >> a;

	return 0;
}