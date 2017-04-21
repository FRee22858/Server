#pragma once
#include <iostream>
#include "TypeDef.h"

class LogManager
{
private:
	LogManager();   //构造函数是私有的  

public:
	~LogManager();

private:
	static LogManager *m_pInstance ;
public:
	static LogManager * GetInstance()
	{
		if (m_pInstance == NULL)  //判断是否第一次调用  
			m_pInstance = new LogManager();
		return m_pInstance;
	}  

private:
	std::string mLogIp;             //日志服务IP
	std::string mLogPort;           //日志服务监听端口
	std::string mLogMaxnum;         //日志服务最多连接数

	std::string mLogPath;			//日志路径
	std::string mLogType;			//日志类型
	std::string mLogOutputFile;		//

	std::string mLogOther;         //设备日志文件夹名
	std::string mLogDefault;	   //默认日志文件夹名
	std::string mLogDB;			   //数据库日志文件夹名
	std::string mLogSSDB;		   //神策数据日志文件夹名

public:
	//日志相关配置
	std::string GetLogPath(void)		{ return mLogPath; };
	std::string GetLogOther(void)		{ return mLogOther; };
	std::string GetLogDefault(void)		{ return mLogDefault; };

	std::string GetLogDB(void)		{ return mLogDB; };
	std::string GetLogSSDB(void)		{ return mLogSSDB; };

	eLogType    GetLogType(void);
	eLogOutPut  GetOutputFile(void);

};

