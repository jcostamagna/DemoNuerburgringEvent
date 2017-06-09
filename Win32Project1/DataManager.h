#pragma once
#include <atomic>
#include <time.h>
#include <vector>
#include "Joystick.h"
#include "PostgreSQL.h"
#include "HTCViveVR.h"
#include "PcarsData.h"
#include <thread>

class DataManager
{
private:
	std::atomic<int> idSession;
	std::atomic<bool> timeToQuit;
	Joystick joy;
	PostgreSQL db;
	LighthouseTracking vr;
	PcarsData pcars;
	time_t timer;
	std::vector<std::thread> threads;
	DataManager(const DataManager& object);
	DataManager& operator=(const DataManager& object);
public:
	DataManager(int id) {
		this->idSession.exchange(id);
		timeToQuit = false;
	}
	void startCollecting(int id);
	bool initializeData();
	void saveData(int id, DIJOYSTATE* js, std::vector<double>& vrData);
	void join();
	~DataManager();
};

