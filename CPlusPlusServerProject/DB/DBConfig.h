#pragma once
#include <string>
using namespace std;
class DBConfig
{
public:
	DBConfig(string ip,string username,string pwd,string port,string name);
	~DBConfig();
public:
	string strIp ="192.168.1.108";
	string strUserName="root";
	string strPassWord="root";
	string strPort="3306";
	string strName="test";
};

