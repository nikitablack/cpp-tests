#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;

int global(int a, float b)
{
	cout << "in global" << endl;
	return a + static_cast<int>(b);
}

struct Functor
{
	Functor()
	{
		int a{ 42 };
	}

	Functor(const Functor&)
	{
		int a{ 42 };
	}

	Functor(Functor&&)
	{
		int a{ 42 };
	}

	int operator()(int a, float b)
	{
		cout << "in functor" << endl;
		return a + static_cast<int>(b);
	}

	~Functor()
	{
		int a{ 42 };
	}
};

struct UserStruct
{
	int member(int a, float b)
	{
		cout << "in member" << endl;
		return a + static_cast<int>(b);
	}

	static int staticMember(int a, float b)
	{
		cout << "in static" << endl;
		return a + static_cast<int>(b);
	}
};

template<typename T>
class Delegate;

template<typename Ret, typename ...Args>
class Delegate<Ret(Args...)>
{
	using CallbackType = Ret(*)(shared_ptr<void>, Args...);

public:
	template<typename T, Ret(T::*funcPtr)(Args...)>
	static Delegate create(shared_ptr<T> obj)
	{
		cout << "create member" << endl;
		return Delegate{ obj, &memberCaller<T, funcPtr> };
	}

	template<Ret(*funcPtr)(Args...)>
	static Delegate create()
	{
		cout << "create global" << endl;
		return Delegate{ nullptr, &globalCaller<funcPtr> };
	}

	template<typename T>
	static Delegate create(shared_ptr<T> t)
	{
		cout << "create functor" << endl;
		return Delegate{ t, &functorCaller<T> };
	}
	
	Ret operator()(Args... args)
	{
		return callback(callee, args...);
	}

	bool operator==(const Delegate& other)
	{
		return callee == other.callee && callback == other.callback;
	}

private:
	shared_ptr<void> callee{ nullptr };
	CallbackType callback{ nullptr };

private:
	Delegate(shared_ptr<void> obj, CallbackType funcPtr) : callee{ obj }, callback{ funcPtr }
	{
	}

	template<typename T, Ret(T::*funcPtr)(Args...)>
	static Ret memberCaller(shared_ptr<void> callee, Args... args)
	{
		return (static_cast<T*>(callee.get())->*funcPtr)(args...);
	}

	template<Ret(*funcPtr)(Args...)>
	static Ret globalCaller(shared_ptr<void>, Args... args)
	{
		return funcPtr(args...);
	}

	template<typename T>
	static Ret functorCaller(shared_ptr<void> functor, Args... args)
	{
		return (*static_cast<T*>(functor.get()))(args...);
	}
};

template<typename T>
class Dispatcher;

template<typename Ret, typename ...Args>
class Dispatcher<Ret(Args...)>
{
public:
	template<Ret(*funcPtr)(Args...)>
	bool add()
	{
		return add(Delegate<Ret(Args...)>::create<funcPtr>());
	}

	template<Ret(*funcPtr)(Args...)>
	bool remove()
	{
		return remove(Delegate<Ret(Args...)>::create<funcPtr>());
	}

	template<typename T, Ret(T::*funcPtr)(Args...)>
	bool add(shared_ptr<T> obj)
	{
		return add(Delegate<Ret(Args...)>::create<T, funcPtr>(obj));
	}

	template<typename T, Ret(T::*funcPtr)(Args...)>
	bool remove(shared_ptr<T> obj)
	{
		return remove(Delegate<Ret(Args...)>::create<T, funcPtr>(obj));
	}

	template<typename T>
	bool add(shared_ptr<T> t)
	{
		return add(Delegate<Ret(Args...)>::create(t));
	}

	template<typename T>
	bool remove(shared_ptr<T> t)
	{
		return remove(Delegate<Ret(Args...)>::create(t));
	}

	void operator()(Args... args)
	{
		isCalling = true;

		for (auto& delegate : delegates)
		{
			delegate(args...);
		};

		isCalling = false;

		for (auto& delegate : delegatesToAdd)
		{
			add(delegate);
		}
		delegatesToAdd.clear();

		for (auto& delegate : delegatesToRemove)
		{
			remove(delegate);
		}
		delegatesToRemove.clear();
	}

	bool add(Delegate<Ret(Args...)> delegate)
	{
		if (find(delegates.begin(), delegates.end(), delegate) != delegates.end())
		{
			return false;
		}

		if (!isCalling)
		{
			delegates.push_back(delegate);
		}
		else
		{
			delegatesToAdd.push_back(delegate);
		}

		return true;
	}

	bool remove(Delegate<Ret(Args...)> delegate)
	{
		auto it = find(delegates.begin(), delegates.end(), delegate);

		if (it == delegates.end())
		{
			return false;
		}

		if (!isCalling)
		{
			delegates.erase(it);
		}
		else
		{
			delegatesToRemove.push_back(delegate);
		}

		return true;
	}
private:
	vector<Delegate<Ret(Args...)>> delegates;
	vector<Delegate<Ret(Args...)>> delegatesToAdd;
	vector<Delegate<Ret(Args...)>> delegatesToRemove;
	bool isCalling{ false };
};

template <typename T, typename L = typename std::decay<T>::type>
std::shared_ptr<L> make_shared_lambda(T&& t)
{
	return make_shared<L>(forward<T>(t));
}

int main()
{
	Dispatcher<int(int, float)> dispatcher;

	{
		cout << "lambda test" << endl;

		auto ptr = make_shared_lambda([](int a, float b)->int
		{
			cout << "in lambda" << endl;
			return a + static_cast<int>(b);
		});

		dispatcher.add(ptr);

		cout << endl;
	}
	
	{
		cout << "lambda with capture test" << endl;

		int toCapture{ 42 };
		auto ptr = make_shared_lambda([toCapture](int a, float b)->int
		{
			cout << "in lambda with capture" << endl;
			return toCapture + a + static_cast<int>(b);
		});

		dispatcher.add(ptr);

		cout << endl;
	}
	
	{
		cout << "global test" << endl;

		dispatcher.add<&global>();

		cout << endl;
	}
	
	{
		cout << "functor test" << endl;

		auto ptr = make_shared<Functor>();

		dispatcher.add(ptr);

		cout << endl;
	}
	
	{
		cout << "member test" << endl;

		auto ptr = make_shared<UserStruct>();

		dispatcher.add<UserStruct, &UserStruct::member>(ptr);

		cout << endl;
	}
	
	{
		cout << "static test" << endl;

		dispatcher.add<&UserStruct::staticMember>();

		cout << endl;
	}
	
	{
		cout << "std::function test" << endl;

		auto ptr = make_shared<function<int(int, float)>>(&global);

		dispatcher.add(ptr);

		cout << endl;

	}

	dispatcher(5, 10.0f);

	cin.get();

	return 0;
}