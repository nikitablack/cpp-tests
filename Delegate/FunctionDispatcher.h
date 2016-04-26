#pragma once

#include <functional>
#include <type_traits>
#include <memory>
#include <string>
#include <vector>

template<size_t>
struct MyPlaceholder {};

namespace std
{
	template<size_t N>
	struct is_placeholder<MyPlaceholder<N>> : public integral_constant<size_t, N> {};
}

template<typename T>
class FunctionDispatcher;

template<typename Ret, typename ...Args>
class FunctionDispatcher<Ret(Args...)>
{
public:
	template<Ret(*funcPtr)(Args...)>
	bool add(const std::string& tag)
	{
		addImpl(tag, funcPtr);
		return true;
	}

	template<typename T, Ret(T::*funcPtr)(Args...)>
	bool add(const std::string& tag, std::shared_ptr<T> obj)
	{
		addImpl(tag, bindImpl(obj.get(), funcPtr, std::index_sequence_for<Args...>{}));
		return true;
	}

	template<typename T>
	bool add(const std::string& tag, std::shared_ptr<T> t)
	{
		addImpl(tag, *t.get());
		return true;
	}

	bool remove(const std::string& tag)
	{
		auto it = find(tags.begin(), tags.end(), tag);
		if (it == tags.end())
		{
			return false;
		}

		auto index{ distance(tags.begin(), it) };
		tags.erase(it);

		delegates.erase(delegates.begin() + index);

		return true;
	}

	void operator()(Args... args)
	{
		for (auto& delegate : delegates)
		{
			delegate(args...);
		};
	}

private:
	bool addImpl(const std::string& tag, std::function<Ret(Args...)> delegate)
	{
		if (find(tags.begin(), tags.end(), tag) != tags.end())
		{
			return false;
		}

		delegates.push_back(delegate);
		tags.push_back(tag);

		return true;
	}

	template <typename T, size_t... Idx>
	std::function<Ret(Args...)> bindImpl(T* obj, Ret(T::*funcPtr)(Args...), std::index_sequence<Idx...>)
	{
		return bind(funcPtr, obj, getPlaceholder<Idx>()...);
	}

	template <size_t N>
	MyPlaceholder<N + 1> getPlaceholder()
	{
		return{};
	}
private:
	std::vector<std::function<Ret(Args...)>> delegates;
	std::vector<std::string> tags;
};