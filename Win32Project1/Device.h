#pragma once

#include <stdio.h>
#include <list>
#include <atomic>

class Device
{
public:

	std::atomic<bool> timeToQuit{ false };

	virtual std::list<int> getData() = 0;

	virtual ~Device()
	{
	}

	void stop() {
		//this->timeToQuit = true;
	}
};

