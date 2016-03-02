#include <memory>
#include <type_traits> // for std::enable_if, std::is_same
#include <vector>
#include <string>
#include <utility> // for std::integer_sequence (since c++14)
#include <iostream>

/**
* Base class for the cheat. Outer code should operate with this type.
*/
class CheatBase
{
public:
	virtual ~CheatBase() {};
	/**
	* Runs the cheat with parameters as array of strings. This parameters should correspond to the c++ types of the binded function.
	*/
	virtual bool call(const std::vector<std::string>& cheatParams) = 0;
};

/**
* Cheat implementation.
*
* Args - list of arguments any type
*/
template<typename ...Args>
class Cheat : public CheatBase
{
public:
	/**
	* Cheat created with a pointer to an object wgich holds the cheat function. Cheat function should return a delegate if the command will be finished later in time (asynchronously) or nullptr if it finished immediately
	*
	* Args - list of arguments any type
	*/
	Cheat(void(* const funcPtr)(Args...)) : func{ funcPtr }
	{

	}

	bool call(const std::vector<std::string>& cheatParams) override
	{
		if (sizeof...(Args) != cheatParams.size())
		{
			// if a number of passed arguments is not equal to a number of parameters declared in cheat return false - this is an error.
			return false;
		}

		callHelper(cheatParams, std::index_sequence_for<Args...>{});

		return true;
	}

private:
	template <std::size_t... Idx>
	void callHelper(const std::vector<std::string>& strArgs, std::index_sequence<Idx...>)
	{
		(func)(fromString<Args>(getStringFromArray<Idx>(strArgs))...);
	}

	/**
	* Need this helper to guarantee the order of parameters
	*/
	template <std::size_t N>
	std::string getStringFromArray(const std::vector<std::string>& strArgs)
	{
		return strArgs[N];
	}

	template<typename T>
	typename std::enable_if<std::is_same<T, int>::value, int>::type fromString(const std::string& str)
	{
		return std::stoi(str);
	}

	template<typename T>
	typename std::enable_if<std::is_same<T, float>::value, float>::type fromString(const std::string& str)
	{
		return std::stof(str);
	}

	template<typename T>
	typename std::enable_if<std::is_same<T, bool>::value, bool>::type fromString(const std::string& str)
	{
		return str == "true" ? true : false;
	}

	template<typename T>
	typename std::enable_if<std::is_same<T, std::string>::value, std::string>::type fromString(const std::string& str)
	{
		return str;
	}

private:
	void(* const func)(Args...);
};

template<typename ...Args>
std::shared_ptr<CheatBase> makeCheat(void(*funcPtr)(Args... arguments))
{
	return std::make_shared<Cheat<Args...>>(funcPtr);
}

void test0()
{
	std::cout << "test0()" << "\n";
}

void test1(int a)
{
	std::cout << "test1(), a = " << a << "\n";
}

void test2(int a, float b)
{
	std::cout << "test2(), a = " << a << ", b = " << b << "\n";
}

void test3(std::string a, bool b)
{
	std::cout << "test3(), a = " << a << ", b = " << std::boolalpha << b << "\n";
}

void test4(double a)
{
	std::cout << "test4(), a = " << a << "\n";
}

int main()
{
	auto cheat0 = makeCheat(&test0);
	cheat0->call({}); // return true, output "test0()"
	cheat0->call({ "42" }); // return false since there's no function test0 which accepts one argument

	auto cheat1 = makeCheat(&test1);
	cheat1->call({ "42" }); // return true, output "test1(), a = 42"
	cheat1->call({ "10", "5" }); // return false since there's no function test1 which accepts two arguments
	// cheat1->call({ "abc" }); // runtime error because std::stoi throws an exception if there's no conversion possible

	auto cheat2 = makeCheat(&test2);
	cheat2->call({ "42", "10.5" }); // return true, output "test2(), a = 42, b = 10.5"
	cheat2->call({ "42" }); // return false since there's no function test2 which accepts one argument

	auto cheat3 = makeCheat(&test3);
	cheat3->call({ "abc", "true" }); // return true, output "test3(), a = "abc", b = true"

	// auto cheat4 = makeCheat(&test4); // compile error because there's no fromString override for double

	return 0;
}
