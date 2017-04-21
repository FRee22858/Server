
#include <string>
#include "MySqlDBConnection.h"
#include "SqlServerDBConnection.h"
#include "DBManager.h"

DBManager::DBManager(void)
{
	string sqltype = "mysql";//frTODO:读配置文件
	if (sqltype == "mysql")//mysql数据库，初始化mysql数据库连接
	{
		_conn = new MySqlDBConnection();
	}
	else if (sqltype == "mssql")//sqlserver，初始化sqlserver数据库
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
		std::cout << "数据库连接断开，尝试重新连接失败" << std::endl;
		ret = db->InitConnect(_conf);
		if (ret)
		{
			std::cout << "数据库连接断开，尝试重新连接成功" << std::endl;
			break;
		}
		count++;
	}
	return ret;
}

void DBManager::Run()
{

}

