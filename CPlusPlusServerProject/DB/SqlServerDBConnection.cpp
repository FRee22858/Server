#include "SqlServerDBConnection.h"
#include "../Log/AppLog.h"
#include "../Utils/StrUtil.h"
#include "../Utils/TimeUtil.h"

SqlServerDBConnection::SqlServerDBConnection(void)
{
	DBCon=new CADODatabase;
}

SqlServerDBConnection::~SqlServerDBConnection(void)
{
	if (DBCon!=NULL)
	{
		delete DBCon;
		DBCon=NULL;
	}
}

bool SqlServerDBConnection::InitConnect( void )
{
	bool ret = false;
	int count = 0;

	//string path = "Provider=SQLOLEDB.1;Persist Security Info=False;User ID=" + PROPMANAGER::GetInstance()->GetDbUsername()+";Password="+PROPMANAGER::GetInstance()->GetDbPwd()
	//	+ ";Initial Catalog=" + PROPMANAGER::GetInstance()->GetDbName() + ";Data Source=" + PROPMANAGER::GetInstance()->GetDbIp();//����sqlserver�ַ�����oledb��ʽ

	//try
	//{
	//	while(!DBCon->IsOpen() && count < 3)
	//	{
	//		DBCon->Open(_bstr_t(path.c_str()));//�������ݿ�
	//		count ++;
	//	}
	//	if(DBCon->IsOpen())
	//		ret = true;
	//}
	//catch (...) 
	//{
	//	APPLOG::GetInstance()->WriteLog(OTHER, LOG_EVENT_ERROR, "�����ݿ����");
	//	return false;
	//}
	return ret;
}

bool SqlServerDBConnection::DisConnect( void )
{
	if (DBCon->IsOpen())
	{
		 DBCon->Close();
	}
	return true;
}

bool SqlServerDBConnection::IsConnected( void )
{
	return DBCon->IsOpen();
}
