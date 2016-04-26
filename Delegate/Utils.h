#pragma once

#include <memory>
#include <type_traits>
#include <utility>

template <typename T, typename L = typename std::decay<T>::type>
std::shared_ptr<L> make_shared_lambda(T&& t)
{
	return std::make_shared<L>(std::forward<T>(t));
}