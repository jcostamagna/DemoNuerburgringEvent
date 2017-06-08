#include "stdafx.h"
#include "Session.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <sstream>
#include <Windows.h>
#include "PostgreSQL.h"


void Session::startCollectingData()
{
	//std::cout << "SESION  ID " << this->id << std::endl;
	this->dataManager->startCollecting(this->id);
}

void Session::endCollectingData()
{
	this->dataManager->join();
	//threadData.join();
	for (auto& th : threads) th.join();

}

void Session::start()
{
	startDataBase();
	this->threads.push_back(std::thread(&Session::startCollectingData, this));
	//std::cout << std::endl;
	//std::cout << "The sesion " << id << " starts fine!" << std::endl;
	//std::cout << std::endl;
}

void Session::end()
{
	endCollectingData();
	endDataBase();
	std::cout << std::endl;
	std::cout << "The sesion " << id << " ends!" << std::endl;
	std::cout << std::endl;
}


Session::~Session()
{
	delete this->dataManager;
}


void Session::startDataBase()
{

	PostgreSQL db;
	if (!db.connect()) { return; }
	bool succes = db.doQuery("CREATE TABLE IF NOT EXISTS Session(Id INTEGER PRIMARY KEY, Start timestamp, Finish timestamp)");

	if (!succes) { return; }

	std::stringstream queryString;
	time(&this->timer);

	SYSTEMTIME time;
	GetSystemTime(&time);
	queryString << "INSERT INTO Session VALUES(" << this->id << ",to_timestamp(" << this->timer << std::setw(3) << std::setfill('0') << time.wMilliseconds << "::double precision / 1000)" << ",NULL)";

	db.doQuery(queryString.str());
}

void Session::endDataBase()
{
	PostgreSQL db;
	if (!db.connect()) { return; }

	std::stringstream queryString;
	time(&this->timer);
	SYSTEMTIME time;
	GetSystemTime(&time);

	queryString << "UPDATE Session SET Finish = to_timestamp(" << this->timer << std::setw(3) << std::setfill('0') << time.wMilliseconds << "::double precision / 1000)" << " WHERE Id = " << this->id;
	db.doQuery(queryString.str());
}
