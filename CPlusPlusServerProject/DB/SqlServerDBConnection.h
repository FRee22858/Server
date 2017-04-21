#pragma once
#include "Ado.h"
#include "DBConnection.h"

class SqlServerDBConnection:public DBConnection
{
public:
	SqlServerDBConnection(void);
	~SqlServerDBConnection(void);
public:
	bool InitConnect(void);//初始化sqlserver数据库连接
	bool DisConnect(void);//断开sqlserver数据库连接
	bool IsConnected(void);

private:
	CADODatabase* DBCon;
};
