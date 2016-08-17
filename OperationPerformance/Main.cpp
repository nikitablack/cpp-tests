#define _CRT_SECURE_NO_WARNINGS
#include <chrono>
#include <iostream>
#include <memory>
#include <immintrin.h>
#include <set>
#include <thread>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

const int NUM_TESTS{ 10 };
const int NUM_ITERATIONS{ 10'000'000 };

int testCall()
{
	return 42;
}

struct S
{
	int testCall()
	{
		return 42;
	}
};

struct A
{
	virtual int testCall()
	{
		return 42;
	}
};

struct B : public A
{
	int testCall() override
	{
		return 42;
	}
};

int floatAddition()
{
	volatile float result{ 0.0f };
	volatile float a{ 21.97f };
	volatile float b{ 15.3f };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += (a + b);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "float addition " << t / NUM_TESTS << "\n";

	return static_cast<int>(result);
}

int intDivision()
{
	volatile int result{ 0 };
	volatile int a{ 10 };
	volatile int b{ 3 };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += (a / b);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "int division " << t / NUM_TESTS << "\n";

	return result;
}

int functionCall()
{
	volatile int result{ 0 };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += testCall();
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "function call " << t / NUM_TESTS << "\n";

	return result;
}

int methodCall()
{
	volatile int result{ 0 };
	shared_ptr<S> s{ make_shared<S>() };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += s->testCall();
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "method call " << t / NUM_TESTS << "\n";

	return result;
}

int virtualMethodCall()
{
	volatile int result{ 0 };
	shared_ptr<A> s{ make_shared<B>() };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += s->testCall();
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "virtual method call " << t / NUM_TESTS << "\n";

	return result;
}

int simdFloatAddition()
{
	volatile float result{ 0.0f };

	__m128 a = _mm_set_ps(-5.0f, 42.56f, 13.5f, 0.11557f);
	__m128 b = _mm_set_ps(98.6f, 68.759f, -45.3f, 897.6f);

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			float* res = (float*)&_mm_sub_ps(a, b);
			result += res[0];
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "simd float addition " << t / NUM_TESTS << "\n";

	return static_cast<int>(result);
}

int intComparison()
{
	volatile int result{ 0 };
	volatile int a{ 10 };
	volatile int b{ 3 };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += a > b ? 1 : 2;
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "int comparison " << t / NUM_TESTS << "\n";

	return result;
}

int floatComparison()
{
	volatile float result{ 0.0f };
	volatile float a{ 10.0f };
	volatile float b{ 10.1f };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += a > b ? 1.0f : 2.0f;
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "float comparison " << t / NUM_TESTS << "\n";

	return static_cast<int>(result);
}

int setLookupFirst()
{
	volatile int result{ 0 };

	set<int> s;
	for (int i{ 0 }; i < 16; ++i)
	{
		s.insert(i);
	}

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += *s.find(0);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "set lookup first " << t / NUM_TESTS << "\n";

	return result;
}

int setLookupLast()
{
	volatile int result{ 0 };

	set<int> s;
	for (int i{ 0 }; i < 16; ++i)
	{
		s.insert(i);
	}

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += *s.find(15);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "set lookup last " << t / NUM_TESTS << "\n";

	return result;
}

int arrayLookupFirst()
{
	volatile int result{ 0 };

	int arr[16];
	for (int i{ 0 }; i < 16; ++i)
	{
		arr[i] = i;
	}

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += arr[0];
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "array lookup first " << t / NUM_TESTS << "\n";

	return result;
}

int arrayLookupLast()
{
	volatile int result{ 0 };

	int arr[16];
	for (int i{ 0 }; i < 16; ++i)
	{
		arr[i] = i;
	}

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += arr[15];
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "array lookup last " << t / NUM_TESTS << "\n";

	return result;
}

int threadCreation()
{
	volatile int result{ 0 };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < 100/*NUM_ITERATIONS*/; ++j)
		{
			thread t{&testCall};
			t.detach();
			++result;
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "thread creation " << t / NUM_TESTS << "\n";

	return result;
}

int mallocTest()
{
	volatile int result{ 0 };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			int* ptr{ (int*)malloc(sizeof(int)) };
			++result;
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "malloc " << t / NUM_TESTS << "\n";

	return result;
}

int printfTest()
{
	volatile int result{ 0 };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += printf("");
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "printf " << t / NUM_TESTS << "\n";

	return result;
}

int fopenTest()
{
	volatile int result{ 0 };

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			fopen("C:\\Users\\ncherniy\\Desktop\\test\\SpeedTest\\SpeedTest\\Main.cpp", "r");
			++result;
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "fopen " << t / NUM_TESTS << "\n";

	return result;
}

int strcmpTest()
{
	volatile int result{ 0 };

	char a[64];
	char b[64];

	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += strcmp(a, b);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
	}
	cout << "strcmp " << t / NUM_TESTS << "\n";

	return result;
}

int main()
{
	volatile int result{ 0 };

	for (int i{ 0 }; i < 10; ++i)
	{
		result += floatAddition();
		result += intDivision();
		result += functionCall();
		result += methodCall();
		result += virtualMethodCall();
		result += simdFloatAddition();
		result += intComparison();
		result += floatComparison();
		result += setLookupFirst();
		result += setLookupLast();
		result += arrayLookupFirst();
		result += arrayLookupLast();
		//result += threadCreation(); // very slow
		result += strcmpTest();
		result += mallocTest();
		result += printfTest();
		result += fopenTest();
		cout << '\n';
	}

	cin.get();

	return result;
}