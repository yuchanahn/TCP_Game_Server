#pragma once
#include <chrono>

using namespace std::chrono;

class YCTime
{
	system_clock::time_point* lastT = nullptr;

	system_clock::time_point t;

public:
	static float deltaTime;
	static int WriteTime;

	static YCTime& Instance() {
		static YCTime t;
		return t;
	}
	YCTime();
	~YCTime();

	static void UpdateDeltaTime();





	void TimerStart();
	float TimerEnd();
};
