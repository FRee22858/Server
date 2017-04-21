
#ifndef __DBMANAGER_H_
#define __DBMANAGER_H_

///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DBManager.h
// �����ߣ�FRee
// ����ʱ�䣺2017-04-06
// �������������ݿ���������࣬�ⲿͨ��ͳһ�ӿڲ������ݿ�
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