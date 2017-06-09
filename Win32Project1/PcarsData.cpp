#include "stdafx.h"
#include "PcarsData.h"


PcarsData::PcarsData()
{
	fileHandle = OpenFileMappingA(PAGE_READONLY, FALSE, MAP_OBJECT_NAME);
	if (fileHandle == NULL) { sharedData = NULL; }
	sharedData = (SharedMemory*)MapViewOfFile(fileHandle, PAGE_READONLY, 0, 0, sizeof(SharedMemory));
}

bool PcarsData::validState()
{
	return this->sharedData != NULL;
}

float PcarsData::getCurrentTime()
{
	return sharedData->mCurrentTime;
}




PcarsData::~PcarsData()
{
	if (validState()) UnmapViewOfFile(sharedData);
	if (fileHandle) CloseHandle(fileHandle);
}
