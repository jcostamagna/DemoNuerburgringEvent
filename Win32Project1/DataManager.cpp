#include "stdafx.h"
#include "DataManager.h"
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <chrono>
#include <string>


void DataManager::startCollecting(int id)
{
	if (!initializeData() || !this->pcars.validState()) {
		MessageBox(NULL, L"Couldn't initialize data!", L"Error", NULL);
		return;
	}
	joy.start();
	while (!this->timeToQuit.load()) {

		saveData(id, joy.getJoy(), vr.ParseTrackingFrame());
		Sleep(5);
#ifdef _DEBUG
		//Sleep(200);
#endif
	}
	joy.close();
}

bool DataManager::initializeData()
{
	if (!db.connect()) { return false;}
	bool succes = db.doQuery("CREATE TABLE IF NOT EXISTS DataSteering(time timestamp, IDSESSION INTEGER REFERENCES Session(Id), STEERING INT, ACCELERATOR INT, BRAKE INT"\
		", SLIDER1 INT, PRIMARY KEY (time, IDSESSION) );");

	if (!succes) { return false; }

	succes = db.doQuery("CREATE TABLE IF NOT EXISTS DataVr(time timestamp, IDSESSION INTEGER REFERENCES Session(Id),  PositionX FLOAT, PositionY FLOAT, PositionZ FLOAT"\
		", RotationX FLOAT, RotationY FLOAT, RotationZ FLOAT, PRIMARY KEY (time, IDSESSION) );");

	if (!succes) { return false; }

	succes = db.doQuery("CREATE TABLE IF NOT EXISTS DataTypesPcars(id SERIAL, data_Type VARCHAR, UNIQUE(data_Type));");

	if (!succes) { return false; }

	succes = db.doQuery("CREATE TABLE IF NOT EXISTS DataPcars(id SERIAL, time timestamp, IDSESSION INTEGER REFERENCES Session(Id), data_Type VARCHAR REFERENCES"\
		"  DataTypesPcars(data_Type), value FLOAT);");

	if (!succes) { return false; }

	//succes = db.doQuery("INSERT INTO DataTypesPcars (data_Type) VALUES('Current time')");

	return succes;
}

void DataManager::saveData(int id, DIJOYSTATE * js, std::vector<double>& vrData)
{
	if (js->lY == 0 && js->rglSlider[0] == 0 && js->lRz == 0)
		return;

	//PostgreSQL db;
	//db.connect();
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

	queryString.str("");

	queryString << "INSERT INTO DataPcars (time, IDSESSION, data_Type, value) VALUES( to_timestamp(" << this->timer << std::setw(3) << std::setfill('0') << time.wMilliseconds
		<< "::double precision / 1000)," << id <<", 'Current time'," << this->pcars.getCurrentTime() <<  ")";
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
