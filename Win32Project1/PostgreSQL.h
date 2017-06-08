#pragma once
#include <libpq-fe.h>
#include <stdio.h>
#include <string>

class PostgreSQL
{
private:
	PGconn *dbconn;
	std::string& dbname, user, password, hostaddr, port;
	PGresult *query;
	bool finish;
	void endQueryError();
	PostgreSQL(const PostgreSQL& object);
	PostgreSQL& operator=(const PostgreSQL& object);
public:
	PostgreSQL(): dbname(std::string("postgres")), user(std::string("postgres")), password(std::string("1way.Street")), hostaddr(std::string("127.0.0.1")),
					port(std::string("5432")), query(NULL), finish(false) {}

	bool connect();
	bool doQuery(std::string& queryString);
	bool doQuery(const char* queryString);
	bool doQuery(char* queryString);
	bool doQuerySelect(const char* queryString);
	std::string getValue(int tup_num, int fiel_num);
	~PostgreSQL();
};

