#pragma once

#include <vector>
#include "Delegate.h"

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
	bool add(std::shared_ptr<T> obj)
	{
		return add(Delegate<Ret(Args...)>::create<T, funcPtr>(obj));
	}

	template<typename T, Ret(T::*funcPtr)(Args...)>
	bool remove(std::shared_ptr<T> obj)
	{
		return remove(Delegate<Ret(Args...)>::create<T, funcPtr>(obj));
	}

	template<typename T>
	bool add(std::shared_ptr<T> t)
	{
		return add(Delegate<Ret(Args...)>::create(t));
	}

	template<typename T>
	bool remove(std::shared_ptr<T> t)
	{
		return remove(Delegate<Ret(Args...)>::create(t));
	}

	void operator()(Args... args)
	{
		for (auto& delegate : delegates)
		{
			delegate(args...);
		};
	}

	bool add(Delegate<Ret(Args...)> delegate)
	{
		if (find(delegates.begin(), delegates.end(), delegate) != delegates.end())
		{
			return false;
		}

		delegates.push_back(delegate);

		return true;
	}

	bool remove(Delegate<Ret(Args...)> delegate)
	{
		auto it = find(delegates.begin(), delegates.end(), delegate);
		if (it == delegates.end())
		{
			return false;
		}

		delegates.erase(it);

		return true;
	}
private:
	std::vector<Delegate<Ret(Args...)>> delegates;
};