#pragma once
#include <atomic>
#include <time.h>
#include <vector>
#include "Joystick.h"

class DataManager
{
private:
	std::atomic<int> idSession;
	std::atomic<bool> timeToQuit;
	DataManager(const DataManager& object);
	DataManager& operator=(const DataManager& object);
	Joystick joy;
	time_t timer;
public:
	DataManager(int id) {
		this->idSession.exchange(id);
		timeToQuit = false;
	}
	void startCollecting(int id);
	void initializeData();
	void saveData(int id, DIJOYSTATE* js, std::vector<double>& vrData);
	void join();
	~DataManager();
};

