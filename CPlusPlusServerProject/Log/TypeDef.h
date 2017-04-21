#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_
///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TypeDef.h
// �����ߣ�FRee
// ����ʱ�䣺2017-04-06
// �����������Զ������Ͷ��壬����ദʹ�õ��Զ�������ͳһ�ڴ˴�����
///////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <list>
#include <vector>
#include <map>

//��־���Ͷ���
typedef enum eLogType              
{
	LOG_EVENT_DEBUG = 0,		//������Ϣ
	LOG_EVENT_WARNNING = 1,		//������Ϣ
	LOG_EVENT_ERROR = 2,		//������Ϣ
	LOG_EVENT_INFO = 3			//�����Ϣ
}enumLogType;

//��־�������
typedef enum eLogOutPut
{
	LOG_OUTPUT_UNDEFINE = 0,	//δ�����������
	LOG_OUTPUT_SCREEN	= 1,	//�������Ļ
	LOG_OUTPUT_FILE		= 2,	//������ļ�
	LOG_OUTPUT_BOTH		= 3		//�������Ļ���ļ�
}enumLogOutPut;

//ģ�����Ͷ���
typedef enum eModuleType
{
	NONE = 0,	//�����ͣ����ڴ���
	OTHER = 1,	//����
	DB = 2,		//���ݿ�
	SSDB=3,		//�������	
	//frTODO: ����ģ���������
}enumModuleType;

#endif
