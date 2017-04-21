#pragma once
#include "../IServer/IServer.h"
#include "../DB/DBManager.h"

class Api :
	public IServer
{
public:
	Api();
	~Api();
public:
	void Init(char* argv[]);
	/// <summary>
	/// �ر��˳�������
	/// </summary>
	void Exit();
	/// <summary>
	/// ��ѭ��
	/// </summary>
	void Run();
	/// <summary>
	/// ����
	/// </summary>
	void ProcessInput();

private:
	void InitDB();
private:
	DBManager *_db;
};

