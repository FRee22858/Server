#include "DBConfig.h"



DBConfig::DBConfig(string ip, string username, string pwd, string port, string name)
{
	strIp = ip;
	strUserName = username;
	strPassWord = pwd;
	strPort = port;
	strName = name;
}

DBConfig::~DBConfig()
{
}

