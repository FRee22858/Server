#include "MySqlDBConnection.h"
#include "MySql.h"
#include "../Log/AppLog.h"
#include "../Utils/StrUtil.h"
#include "../Utils/TimeUtil.h"
#include "DBConfig.h"

MySqlDBConnection::MySqlDBConnection(void)
{
	DBCon=new MySql;
}

MySqlDBConnection::~MySqlDBConnection(void)
{
	if (DBCon!=NULL)
	{
		delete DBCon;
		DBCon=NULL;
	}
}

bool MySqlDBConnection::InitConnect(DBConfig *conf)
{
	bool ret=false;
	int count=0;
	try
	{
		while ((count<3)&&(!DBCon->BConnIsConnected()))
		{
			DBCon->Server(conf->strIp);//数据库ip
			DBCon->Username(conf->strUserName);//数据库用户名
			DBCon->Password(conf->strPassWord);//数据库密码
			DBCon->Port(conf->strPort);//数据库端口
			DBCon->Database(conf->strName);//数据库名字
			DBCon->BConnByStr();//连接数据库
			count ++;
		}
		if(DBCon->BConnIsConnected())
			ret = true;			
		return ret;
	}

	catch (...)
	{
		APPLOG::GetInstance()->WriteLog(DB, LOG_EVENT_ERROR, "打开数据库出错");
		return false;
	}
}
bool MySqlDBConnection::DisConnect(void)
{
	bool ret=false;
	if (DBCon->BConnIsConnected())
	{
		ret= DBCon->BConnDisConnect();//断开数据库
	}
	return ret;
}
bool MySqlDBConnection::IsConnected( void )
{
	return DBCon->BConnIsConnected();
}

