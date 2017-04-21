#ifndef _APPLOG_H_
#define _APPLOG_H_
///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����AppLog.h
// �����ߣ�FRee
// ����ʱ�䣺2017-04-06
// ������������־��¼
// ��ע��ʵ��������־��¼��ʽ��
// 1�����������ļ�ȷ����Ϣ�������ʽ����Ļ���ļ���
// 2������һ���Ըı���־��Ϣ�����ʽ
// 3������̬�ı���־��Ϣ�����ʽ
// 4�������ʽѡ�����ȼ��𣺶�̬�ı� > һ���Ըı� > �����ļ�
///////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "TypeDef.h"

typedef class AppLog
{
private:
	AppLog();   //���캯����˽�е�  

public:
	~AppLog();

private:
	static AppLog *m_pInstance;
public:
	static AppLog * GetInstance()
	{
		if (m_pInstance == NULL)  //�ж��Ƿ��һ�ε���  
			m_pInstance = new AppLog();
		return m_pInstance;
	}
private:
	eLogType mLogType;  //��־����
	enumLogOutPut mLogOutPut; //��־�������
	enumLogOutPut mOldLogOutPut; //�������ļ���ʼ��������
private:
	//����ģ�����ƻ��log�ļ����ƣ�����·��������Ӧ��ģ���ַ��������Լ���־��Ϣ��������
	void GetLogBasicInfo(eModuleType Module,eLogType LogType,std::string& strLogName,std::string& strModule,std::string& strLogTpye);
	bool WriteLogToFile(eModuleType Module,eLogType LogType,const std::string& EventLog);
	//����ӿ�
public:
	eLogType GetLogType(void){return mLogType;};						//�õ���־������
	bool SetLogType(eLogType LogType){mLogType=LogType;return true;};   //������־������

public:
	void ManualSetOutType(enumLogOutPut LogOutPut){mLogOutPut = LogOutPut;};	//�ֶ��ı���־�����ʽ
	void CloseManualSet(){mLogOutPut = mOldLogOutPut;};							//�ر��ֶ��ı�������ʽ����ԭΪĬ��

	bool WriteLog(eModuleType Module,eLogType LogType,const std::string& EventLog,enumLogOutPut LogOutPut = LOG_OUTPUT_UNDEFINE);
}APPLOG;

#endif
