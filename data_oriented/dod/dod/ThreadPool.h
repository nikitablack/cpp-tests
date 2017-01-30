#pragma once

// https://github.com/progschj/ThreadPool/blob/master/ThreadPool.h

#include <array>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
	ThreadPool(size_t);
	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args)
		->std::future<typename std::result_of<F(Args...)>::type>;
	~ThreadPool();
	size_t getSize();

	template<uint32_t N, typename C, typename F>
	void runTasksSync(C&& cont, F&& task, std::size_t num);
private:
	// need to keep track of threads so we can join them
	std::vector< std::thread > workers;
	// the task queue
	std::queue< std::function<void()> > tasks;

	// synchronization
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
	size_t size;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
	: stop(false), size{ threads }
{
	for (size_t i = 0; i<threads; ++i)
		workers.emplace_back(
			[this]
	{
		for (;;)
		{
			std::function<void()> task;

			{
				std::unique_lock<std::mutex> lock(this->queue_mutex);
				this->condition.wait(lock,
					[this] { return this->stop || !this->tasks.empty(); });
				if (this->stop && this->tasks.empty())
					return;
				task = std::move(this->tasks.front());
				this->tasks.pop();
			}

			task();
		}
	}
	);
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		// don't allow enqueueing after stopping the pool
		if (stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");

		tasks.emplace([task]() { (*task)(); });
	}
	condition.notify_one();
	return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread &worker : workers)
		worker.join();
}

inline size_t ThreadPool::getSize()
{
	return size;
}

template<uint32_t N, typename C, typename F>
void ThreadPool::runTasksSync(C&& cont, F&& task, std::size_t num)
{
	std::array<std::future<void>, N> results;

	uint32_t numPerThread{ static_cast<uint32_t>(ceil(static_cast<float>(num) / (N + 1))) };
	for (size_t t{ 0 }; t < N; ++t)
	{
		size_t indStart{ t * numPerThread };
		size_t indEnd{ indStart + numPerThread - 1 };

		std::future<void> result{ enqueue(
			[indStart, indEnd, &cont, &task]()
		{
			for (size_t i{ indStart }; i <= indEnd; ++i)
			{
				task(cont, i);
			}
		}
		) };

		results[t] = move(result);
	}

	for (size_t i{ numPerThread * N }; i < num; ++i)
	{
		task(cont, i);
	}

	for (std::future<void>& result : results)
	{
		result.get();
	}
}