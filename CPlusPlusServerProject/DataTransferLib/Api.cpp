#include "Api.h"
#include <windows.h>  
#include <process.h>   

Api::Api()
{
}


Api::~Api()
{
}

void Api::Init(char* argv[])
{
	InitDB();
}
/// <summary>
/// 关闭退出服务器
/// </summary>
void Api::Exit()
{
}
/// <summary>
/// 主循环
/// </summary>
void Api::Run()
{

}
/// <summary>
/// 输入
/// </summary>
void Api::ProcessInput()
{
}

// 子线程函数
DWORD __stdcall DBThreadFun(LPVOID pM)
{
	DBManager *db = (DBManager*)pM;
	printf("****DBThreadFun线程!***/n");
	db->Run();
	return 0;
}

void Api::InitDB()
{
	//frTODO: 初始化DB
	DBConfig *config = new DBConfig("192.168.1.108", "root", "root", "3306", "test");
	_db = new DBManager();
	_db->InitConnect(config);
	HANDLE hThread = CreateThread(NULL, 0, DBThreadFun, _db, 0, NULL);

}