
#ifndef __DBMANAGER_H_
#define __DBMANAGER_H_

///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DBManager.h
// 创建者：FRee
// 创建时间：2017-04-06
// 内容描述：数据库操作管理类，外部通过统一接口操作数据库
///////////////////////////////////////////////////////////////////////////////////////////

#include "DBParamTypeDef.h"
#include "AbstractDBQuery.h"
#include "DBConfig.h"
#include <queue>
using namespace std;

class DBManager
{
public:
	DBManager(void);
	virtual ~DBManager(void);
public:
	bool InitConnect(DBConfig *config);
	bool DisConnect(void);
	bool IsConnected(void);
	bool RetryConnect(DBConnection* db);

private:
	DBConnection* _conn;
	DBConfig *_conf;
	queue <AbstractDBQuery> *saveQueue;

public:
	void Run();
	void Add(AbstractDBQuery query);
};
#endif