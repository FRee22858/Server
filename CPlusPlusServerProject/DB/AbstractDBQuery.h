#pragma once
#include "DBConnection.h"
#include <string>
using namespace std;
class AbstractDBQuery
{
public:
	AbstractDBQuery();
	virtual ~AbstractDBQuery();
protected:
	DBConnection *dbcon;
	char *sqlCmd;
public:
	virtual bool Execute();
};

