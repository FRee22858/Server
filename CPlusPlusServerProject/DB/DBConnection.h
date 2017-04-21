
#ifndef __DBCONNECTION__
#define __DBCONNECTION__
///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：DBConnection.h
// 创建者：FRee
// 创建时间：2017-05-27
// 内容描述：数据库连接操作纯虚基类
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
	virtual bool InitConnect(DBConfig *conf){return true;};//初始化数据库连接
	virtual bool DisConnect(void){return true;};//断开数据库连接
	virtual bool IsConnected(void){return true;};
};

#endif
