
#include <string>
#include "MySqlDBConnection.h"
#include "SqlServerDBConnection.h"
#include "DBManager.h"

DBManager::DBManager(void)
{
	string sqltype = "mysql";//frTODO:�������ļ�
	if (sqltype == "mysql")//mysql���ݿ⣬��ʼ��mysql���ݿ�����
	{
		_conn = new MySqlDBConnection();
	}
	else if (sqltype == "mssql")//sqlserver����ʼ��sqlserver���ݿ�
	{
		_conn = new SqlServerDBConnection();
	}
	else
	{
		//...
	}
}

DBManager::~DBManager(void)
{
	if (_conn)
	{
		delete _conn;
		_conn = NULL;
	}
}

bool DBManager::InitConnect(DBConfig *config)
{
	_conf = config;
	return _conn->InitConnect(config);
}

bool DBManager::DisConnect(void)
{
	return _conn->DisConnect();
}

bool DBManager::IsConnected(void)
{
	return _conn->IsConnected();
}

bool DBManager::RetryConnect(DBConnection* db)
{
	int count = 0;
	bool ret = false;
	while (count < 3)
	{
		std::cout << "���ݿ����ӶϿ���������������ʧ��" << std::endl;
		ret = db->InitConnect(_conf);
		if (ret)
		{
			std::cout << "���ݿ����ӶϿ��������������ӳɹ�" << std::endl;
			break;
		}
		count++;
	}
	return ret;
}

void DBManager::Run()
{

}

