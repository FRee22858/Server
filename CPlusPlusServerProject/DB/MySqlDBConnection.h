#ifndef __MYSQLDBCONNECTION__
#define __MYSQLDBCONNECTION__

///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：MySqlDBConnection.h
// 创建者：free
// 创建时间：2017-04-13
// 内容描述：MySql数据库操作类
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
