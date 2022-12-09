#pragma once

#include <vector>
#include <mutex>
#include <thread>
#include <functional>
using namespace std;

/*------------------
	ThreadManager
-------------------*/

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void	Launch(function<void(void)> callback);
	void	Join();

	static void InitTLS();
	static void DestroyTLS();

private:
	mutex			_lock;
	vector<thread>	_threads;
};

extern thread_local unsigned int    LThreadId;