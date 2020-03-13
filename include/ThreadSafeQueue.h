#ifndef SAFE_QUEUE
#define SAFE_QUEUE

#include <queue>
#include <mutex>
#include <condition_variable>

// A threadsafe-queue.
template <class T>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue()
		: q()
		, m()
		, c()
	{}

	~ThreadSafeQueue()
	{}

	// Add an element to the queue.
	void enqueue(T t)
	{
		std::lock_guard<std::mutex> lock(m);
		q.push(t);
		c.notify_one();
	}

	int size()
	{
		std::lock_guard<std::mutex> lock(m);
		return q.size();
	}

	// Get the "front"-element.
	// If the queue is empty, wait till a element is avaiable.
	T dequeue()
	{
		std::unique_lock<std::mutex> lock(m);
		c.wait(lock, [&] { return !q.empty(); });
		T val = q.front();
		q.pop();
		return val;
	}

private:
	std::queue<T> q;
	mutable std::mutex m;
	std::condition_variable c;
};
#endif