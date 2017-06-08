#include "stdafx.h"
#include "DataManager.h"
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <chrono>
#include <string>
#include "PostgreSQL.h"
#include "HTCViveVR.h"

void DataManager::startCollecting(int id)
{
	LighthouseTracking vr;
	initializeData();
	joy.start();
	while (!this->timeToQuit.load()) {

		//std::cout << "SESION:" << this->idSession.load() << std::endl;
		saveData(id, joy.getJoy(), vr.ParseTrackingFrame());
		//Sleep(5);
#ifdef _DEBUG
		//Sleep(200);
#endif
	}
	joy.close();
	//std::cout << "TERMINE" << std::endl;
}

void DataManager::initializeData()
{
	PostgreSQL db;
	if (!db.connect()) { return;}
	bool succes = db.doQuery("CREATE TABLE IF NOT EXISTS DataSteering(time timestamp, IDSESSION INTEGER REFERENCES Session(Id), STEERING INT, ACCELERATOR INT, BRAKE INT"\
		", SLIDER1 INT, PRIMARY KEY (time, IDSESSION) );");

	if (!succes) { return; }

	succes = db.doQuery("CREATE TABLE IF NOT EXISTS DataVr(time timestamp, IDSESSION INTEGER REFERENCES Session(Id),  PositionX FLOAT, PositionY FLOAT, PositionZ FLOAT"\
		", RotationX FLOAT, RotationY FLOAT, RotationZ FLOAT, PRIMARY KEY (time, IDSESSION) );");

	if (!succes) { return; }
}

void DataManager::saveData(int id, DIJOYSTATE * js, std::vector<double>& vrData)
{
	if (js->lY == 0 && js->rglSlider[0] == 0 && js->lRz == 0)
		return;

	PostgreSQL db;
	db.connect();
	std::stringstream queryString;
	time(&this->timer);

	SYSTEMTIME time;
	GetSystemTime(&time);
//#ifdef _DEBUG
	
	//std::cout << "VR DATA: ";
	//for (auto i = vrData.begin(); i != vrData.end(); ++i)
	//	std::cout << *i << ' ';
	//std::cout << std::endl;
	//std::cout << std::endl << "Steering Wheel DATA:" << js->lX << "," << js->lY << "," << js->rglSlider[0] << "," << js->lRz << std::endl;
	
//#endif



	queryString << "INSERT INTO DataSteering VALUES( to_timestamp(" << this->timer << std::setw(3) << std::setfill('0') << time.wMilliseconds << "::double precision / 1000)," << id <<
		"," << js->lX << "," << js->lY << "," << js->lRz << "," << js->rglSlider[0] <<")";
	db.doQuery(queryString.str());

	//std::cout << queryString.str() << std::endl;
	queryString.str("");
	

	queryString << "INSERT INTO DataVr VALUES( to_timestamp(" << this->timer << std::setw(3) << std::setfill('0') << time.wMilliseconds << "::double precision / 1000)," << id <<
		"," << vrData.at(0) << "," << vrData.at(1) << "," << vrData.at(2) << "," << vrData.at(3) << "," << vrData.at(4) << "," << vrData.at(5) << ")";
	db.doQuery(queryString.str());


}

void DataManager::join()
{
	//std::cout << "TERMINANDO" << std::endl;
	timeToQuit = true;
}

DataManager::~DataManager()
{
}
