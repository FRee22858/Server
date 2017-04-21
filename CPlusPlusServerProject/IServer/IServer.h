//#pragma once
#ifndef _ISERVER_H_
#define _ISERVER_H_
#include <string>

typedef enum Mode
{
	/// <summary>
	/// 手动
	/// </summary>
	Manual = 0,
	/// <summary>
	/// 自动
	/// </summary>
	Auto = 1,
}MODE;
class IServer
{
public:
	IServer();
	virtual ~IServer();
	/// <summary>
	/// 开启方式
	/// </summary>
	MODE StartMode;
	/// <summary>
	/// 服务器名称
	/// </summary>
	std::string ServerName;
	/// <summary>
	/// 初始化服务器信息
	/// </summary>
	/// <param name="argv"></param>
	virtual void Init(char* argv[]) = 0;
	/// <summary>
	/// 关闭退出服务器
	/// </summary>
	virtual void Exit() = 0;
	/// <summary>
	/// 主循环
	/// </summary>
	virtual void Run() = 0;
	/// <summary>
	/// 输入
	/// </summary>
	virtual void ProcessInput() = 0;
};
#endif;