//#pragma once
#ifndef _ISERVER_H_
#define _ISERVER_H_
#include <string>

typedef enum Mode
{
	/// <summary>
	/// �ֶ�
	/// </summary>
	Manual = 0,
	/// <summary>
	/// �Զ�
	/// </summary>
	Auto = 1,
}MODE;
class IServer
{
public:
	IServer();
	virtual ~IServer();
	/// <summary>
	/// ������ʽ
	/// </summary>
	MODE StartMode;
	/// <summary>
	/// ����������
	/// </summary>
	std::string ServerName;
	/// <summary>
	/// ��ʼ����������Ϣ
	/// </summary>
	/// <param name="argv"></param>
	virtual void Init(char* argv[]) = 0;
	/// <summary>
	/// �ر��˳�������
	/// </summary>
	virtual void Exit() = 0;
	/// <summary>
	/// ��ѭ��
	/// </summary>
	virtual void Run() = 0;
	/// <summary>
	/// ����
	/// </summary>
	virtual void ProcessInput() = 0;
};
#endif;