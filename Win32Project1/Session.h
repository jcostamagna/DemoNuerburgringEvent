#pragma once
#include <time.h>
#include "DataManager.h"
#include <thread>
#include <vector>

class Session
{
private:
	int id;
	time_t timer;
	void startDataBase();
	void endDataBase();
	std::vector<std::thread> threads;
	DataManager* dataManager;
	//std::thread threadData;
	Session(const Session& object);
	Session& operator=(const Session& object);
public:
	Session(int id) : id(id) {
		this->dataManager = new DataManager(id);
	}
	void startCollectingData();
	void endCollectingData();
	void start();
	void end();
	~Session();
};

