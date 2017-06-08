#pragma once

#include <stdio.h>
#include <list>

class Device
{
public:

	//Device();

	virtual std::list<int> getData() = 0;

	virtual ~Device()
	{
	}
};

