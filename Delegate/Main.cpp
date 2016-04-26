#include "FunctionDispatcher.h"
#include "Dispatcher.h"
#include "Utils.h"
#include <iostream>
#include <chrono>

using namespace std;

int global(int a, float b)
{
	return a + static_cast<int>(b);
}

struct UserStruct
{
	int member(int a, float b)
	{
		return a + static_cast<int>(b);
	}
};

int main()
{
	const int NUM_TESTS{ 10 };
	const int NUM_ITERATIONS{ 100'000'00 };

	Dispatcher<int(int, float)> dispatcher;
	FunctionDispatcher<int(int, float)> functionDispatcher;

	auto ptr = make_shared_lambda([](int a, float b)->int
	{
		return a + static_cast<int>(b);
	});

	volatile int result{ 0 };

	cout << "lambda test raw" << "\n";

	dispatcher.add(ptr);
	functionDispatcher.add("lambda", ptr);

	// lambda test raw
	uint64_t t{ 0 };
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += (*ptr)(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "lambda test dispatcher" << "\n";

	// lambda test dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			dispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "lambda test function dispatcher" << "\n";

	// lambda test function dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			functionDispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	dispatcher.remove(ptr);
	functionDispatcher.remove("lambda");

	cout << "global test raw" << "\n";

	dispatcher.add<&global>();
	functionDispatcher.add<&global>("global");

	// global test raw
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += global(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "global test dispatcher" << "\n";

	// global test dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			dispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "global test function dispatcher" << "\n";

	// lambda test function dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			functionDispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	dispatcher.remove<&global>();
	functionDispatcher.remove("global");

	cout << "member test raw" << "\n";

	auto ptr2 = make_shared<UserStruct>();
	dispatcher.add<UserStruct, &UserStruct::member>(ptr2);
	functionDispatcher.add<UserStruct, &UserStruct::member>("UserStruct::member", ptr2);

	// member test raw
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			result += ptr2->member(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "member test dispatcher" << "\n";

	// member test dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			dispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";

	cout << "member test function dispatcher" << "\n";

	// member test function dispatcher
	t = 0;
	for (int i{ 0 }; i < NUM_TESTS; ++i)
	{
		auto start = chrono::high_resolution_clock::now();

		for (int j{ 0 }; j < NUM_ITERATIONS; ++j)
		{
			functionDispatcher(5, 10.0f);
		}

		auto end = chrono::high_resolution_clock::now();
		auto time = chrono::duration_cast<chrono::milliseconds>(end - start);

		t += time.count();
		//cout << time.count() << "\n";
	}
	cout << t / NUM_TESTS << "\n";
	
	dispatcher.remove<UserStruct, &UserStruct::member>(ptr2);
	functionDispatcher.remove("UserStruct::member");

	/*dispatcher.add(ptr);
	functionDispatcher.add("lambda", ptr);
	dispatcher.add<&global>();
	functionDispatcher.add<&global>("global");
	dispatcher.add<UserStruct, &UserStruct::member>(ptr2);
	functionDispatcher.add<UserStruct, &UserStruct::member>("UserStruct::member", ptr2);

	dispatcher(5, 10.0f);
	cout << "\n";
	functionDispatcher(5, 10.0f);*/

	cin.get();

	return result;
}