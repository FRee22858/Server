// DataTransfer.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "../DataTransferLib/Api.h"
#include "../Log/AppLog.h"
#include <windows.h>  
#include <process.h>   
#include <string>

// 子线程函数
DWORD __stdcall ThreadFun(LPVOID pM)
{
	Api *api = (Api*)pM;
	printf("***MainLoop线程ID号为：%d\n***MainLoopThread输出 Action！！！\n", GetCurrentThreadId());
	printf("***MainLoop线程ID号为：%d\n***MainLoopThread输出 Action！！！\n", GetCurrentThreadId());
	api->Run();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	Api* api = new Api();
	try
	{
		api->Init(argv);
	}
	catch (...)
	{
		std::string msg = "";
		msg += "Server init failed!" + msg + " !!";
		APPLOG::GetInstance()->WriteLog(NONE, LOG_EVENT_ERROR, msg.c_str());
		api->Exit();
	}

	try
	{
		HANDLE hThread = CreateThread(NULL, 0, ThreadFun, api, 0, NULL);
		APPLOG::GetInstance()->WriteLog(NONE, LOG_EVENT_INFO, "Server OnReady..");

		DWORD ExitCode = STILL_ACTIVE;
		while (ExitCode == STILL_ACTIVE)
		{
			GetExitCodeThread(hThread, &ExitCode);
			api->ProcessInput();
			Sleep(1000);
		}
	}
	catch (...)
	{
		APPLOG::GetInstance()->WriteLog(NONE, LOG_EVENT_ERROR, "_tmain error!");
	}

	APPLOG::GetInstance()->WriteLog(NONE, LOG_EVENT_INFO, "Server Exit");

	return 0;
}