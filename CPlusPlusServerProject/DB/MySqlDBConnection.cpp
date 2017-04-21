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
			DBCon->Server(conf->strIp);//���ݿ�ip
			DBCon->Username(conf->strUserName);//���ݿ��û���
			DBCon->Password(conf->strPassWord);//���ݿ�����
			DBCon->Port(conf->strPort);//���ݿ�˿�
			DBCon->Database(conf->strName);//���ݿ�����
			DBCon->BConnByStr();//�������ݿ�
			count ++;
		}
		if(DBCon->BConnIsConnected())
			ret = true;			
		return ret;
	}

	catch (...)
	{
		APPLOG::GetInstance()->WriteLog(DB, LOG_EVENT_ERROR, "�����ݿ����");
		return false;
	}
}
bool MySqlDBConnection::DisConnect(void)
{
	bool ret=false;
	if (DBCon->BConnIsConnected())
	{
		ret= DBCon->BConnDisConnect();//�Ͽ����ݿ�
	}
	return ret;
}
bool MySqlDBConnection::IsConnected( void )
{
	return DBCon->BConnIsConnected();
}

