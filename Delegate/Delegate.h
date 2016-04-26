#pragma once

#include <memory>

template<typename T>
class Delegate;

template<typename Ret, typename ...Args>
class Delegate<Ret(Args...)>
{
	using CallbackType = Ret(*)(std::shared_ptr<void>, Args...);

public:
	template<typename T, Ret(T::*funcPtr)(Args...)>
	static Delegate create(std::shared_ptr<T> obj)
	{
		return Delegate{ obj, &memberCaller<T, funcPtr> };
	}

	template<Ret(*funcPtr)(Args...)>
	static Delegate create()
	{
		return Delegate{ nullptr, &globalCaller<funcPtr> };
	}

	template<typename T>
	static Delegate create(std::shared_ptr<T> t)
	{
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
	std::shared_ptr<void> callee{ nullptr };
	CallbackType callback{ nullptr };

private:
	Delegate(std::shared_ptr<void> obj, CallbackType funcPtr) : callee{ obj }, callback{ funcPtr }
	{
	}

	template<typename T, Ret(T::*funcPtr)(Args...)>
	static Ret memberCaller(std::shared_ptr<void> callee, Args... args)
	{
		return (static_cast<T*>(callee.get())->*funcPtr)(args...);
	}

	template<Ret(*funcPtr)(Args...)>
	static Ret globalCaller(std::shared_ptr<void>, Args... args)
	{
		return funcPtr(args...);
	}

	template<typename T>
	static Ret functorCaller(std::shared_ptr<void> functor, Args... args)
	{
		return (*static_cast<T*>(functor.get()))(args...);
	}
};