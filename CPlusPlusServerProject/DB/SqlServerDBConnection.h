#pragma once
#include "Ado.h"
#include "DBConnection.h"

class SqlServerDBConnection:public DBConnection
{
public:
	SqlServerDBConnection(void);
	~SqlServerDBConnection(void);
public:
	bool InitConnect(void);//��ʼ��sqlserver���ݿ�����
	bool DisConnect(void);//�Ͽ�sqlserver���ݿ�����
	bool IsConnected(void);

private:
	CADODatabase* DBCon;
};
