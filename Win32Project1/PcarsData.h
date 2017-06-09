#pragma once

#include "SharedMemory.h"
#define MAP_OBJECT_NAME "$pcars$"

class PcarsData
{
private:
	const SharedMemory* sharedData;
	HANDLE fileHandle;
public:
	PcarsData();
	bool validState();
	float getCurrentTime();
	~PcarsData();
};

