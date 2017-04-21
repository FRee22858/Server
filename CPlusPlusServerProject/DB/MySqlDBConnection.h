#ifndef __MYSQLDBCONNECTION__
#define __MYSQLDBCONNECTION__

///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����MySqlDBConnection.h
// �����ߣ�free
// ����ʱ�䣺2017-04-13
// ����������MySql���ݿ������
///////////////////////////////////////////////////////////////////////////////////////////

#include "DBConnection.h"
#include "DBConfig.h"

class MySql;

class MySqlDBConnection : public DBConnection
{
public:    
    MySqlDBConnection(void);
    ~MySqlDBConnection(void);
public:
	bool InitConnect(DBConfig *conf);
	bool DisConnect(void);
	bool IsConnected(void);

private:
	MySql* DBCon;
};

#endif
