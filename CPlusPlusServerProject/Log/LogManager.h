#pragma once
#include <iostream>
#include "TypeDef.h"

class LogManager
{
private:
	LogManager();   //���캯����˽�е�  

public:
	~LogManager();

private:
	static LogManager *m_pInstance ;
public:
	static LogManager * GetInstance()
	{
		if (m_pInstance == NULL)  //�ж��Ƿ��һ�ε���  
			m_pInstance = new LogManager();
		return m_pInstance;
	}  

private:
	std::string mLogIp;             //��־����IP
	std::string mLogPort;           //��־��������˿�
	std::string mLogMaxnum;         //��־�������������

	std::string mLogPath;			//��־·��
	std::string mLogType;			//��־����
	std::string mLogOutputFile;		//

	std::string mLogOther;         //�豸��־�ļ�����
	std::string mLogDefault;	   //Ĭ����־�ļ�����
	std::string mLogDB;			   //���ݿ���־�ļ�����
	std::string mLogSSDB;		   //���������־�ļ�����

public:
	//��־�������
	std::string GetLogPath(void)		{ return mLogPath; };
	std::string GetLogOther(void)		{ return mLogOther; };
	std::string GetLogDefault(void)		{ return mLogDefault; };

	std::string GetLogDB(void)		{ return mLogDB; };
	std::string GetLogSSDB(void)		{ return mLogSSDB; };

	eLogType    GetLogType(void);
	eLogOutPut  GetOutputFile(void);

};

