
#ifndef __DBCONNECTION__
#define __DBCONNECTION__
///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DBConnection.h
// �����ߣ�FRee
// ����ʱ�䣺2017-05-27
// �������������ݿ����Ӳ����������
///////////////////////////////////////////////////////////////////////////////////////////

#include "DBParamTypeDef.h"
#include "DBConfig.h"
#include <string>
#include <vector>
class DBConnection
{
public:    
    DBConnection(void);
    virtual ~DBConnection(void) = 0;

public:
	virtual bool InitConnect(DBConfig *conf){return true;};//��ʼ�����ݿ�����
	virtual bool DisConnect(void){return true;};//�Ͽ����ݿ�����
	virtual bool IsConnected(void){return true;};
};

#endif
