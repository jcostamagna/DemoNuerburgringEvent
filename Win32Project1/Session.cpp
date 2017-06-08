#include "stdafx.h"
#include "Session.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <libpq-fe.h>
#include <thread>
#include <sstream>
#include <Windows.h>


void Session::startCollectingData()
{
	std::cout << "SESION  ID " << this->id << std::endl;
	//std::cout << "SESION  ID (PUNTERO)" << data->id << std::endl;
	//std::cout << "SESION  ID (OBJETO)" << this->id << std::endl;
	//threadData = std::thread (&DataManager::startCollectingData, this->dataManager);
	this->dataManager->startCollecting(this->id);
	//data.startCollecting(200);
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
	//startCollectingData();
	std::cout << std::endl;
	std::cout << "The sesion " << id << " starts fine!" << std::endl;
	std::cout << std::endl;
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
	PGconn *dbconn;
	dbconn = PQconnectdb("dbname = postgres user = postgres password = 1way.Street hostaddr = 127.0.0.1 port = 5432");
	if (PQstatus(dbconn) == CONNECTION_BAD) {
		std::cout << "Unable to connect to database\n" << std::endl;
		return;
	}




	PGresult* query = PQexec(dbconn, "CREATE TABLE IF NOT EXISTS Session(Id INTEGER PRIMARY KEY, Start timestamp, Finish timestamp)");

	if (PQresultStatus(query) != PGRES_COMMAND_OK) {
		std::cerr << PQerrorMessage(dbconn);
		PQclear(query);
		PQfinish(dbconn);
		return;
	}

	PQclear(query);
	std::stringstream queryString;
	time(&this->timer);

	SYSTEMTIME time;
	GetSystemTime(&time);
	queryString << "INSERT INTO Session VALUES(" << this->id << ",to_timestamp(" << this->timer << std::setw(3) << std::setfill('0') << time.wMilliseconds << "::double precision / 1000)" << ",NULL)";

	query = PQexec(dbconn, queryString.str().c_str());

	if (PQresultStatus(query) != PGRES_COMMAND_OK) {
		std::cerr << PQerrorMessage(dbconn);
		PQclear(query);
		PQfinish(dbconn);
		return;
	}

	PQclear(query);
	PQfinish(dbconn);
}

void Session::endDataBase()
{
	PGconn *conn;
	conn = PQconnectdb("dbname = postgres user = postgres password = 1way.Street hostaddr = 127.0.0.1 port = 5432");
	if (PQstatus(conn) == CONNECTION_BAD) {
		std::cout << "Unable to connect to database\n" << std::endl;
		return;
	}

	PGresult *query;

	std::stringstream queryString;
	time(&this->timer);
	SYSTEMTIME time;
	GetSystemTime(&time);

	queryString << "UPDATE Session SET Finish = to_timestamp(" << this->timer << std::setw(3) << std::setfill('0') << time.wMilliseconds << "::double precision / 1000)" << " WHERE Id = " << this->id;
	query = PQexec(conn, queryString.str().c_str());

	PQfinish(conn);
}
