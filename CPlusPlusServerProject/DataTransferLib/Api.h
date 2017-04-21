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
	/// 关闭退出服务器
	/// </summary>
	void Exit();
	/// <summary>
	/// 主循环
	/// </summary>
	void Run();
	/// <summary>
	/// 输入
	/// </summary>
	void ProcessInput();

private:
	void InitDB();
private:
	DBManager *_db;
};

