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
/// �ر��˳�������
/// </summary>
void Api::Exit()
{
}
/// <summary>
/// ��ѭ��
/// </summary>
void Api::Run()
{

}
/// <summary>
/// ����
/// </summary>
void Api::ProcessInput()
{
}

// ���̺߳���
DWORD __stdcall DBThreadFun(LPVOID pM)
{
	DBManager *db = (DBManager*)pM;
	printf("****DBThreadFun�߳�!***/n");
	db->Run();
	return 0;
}

void Api::InitDB()
{
	//frTODO: ��ʼ��DB
	DBConfig *config = new DBConfig("192.168.1.108", "root", "root", "3306", "test");
	_db = new DBManager();
	_db->InitConnect(config);
	HANDLE hThread = CreateThread(NULL, 0, DBThreadFun, _db, 0, NULL);

}