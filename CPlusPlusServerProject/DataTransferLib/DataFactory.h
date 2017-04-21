#pragma once
#include <string>
#include <queue>
using namespace std;
class DataFactory
{
public:
	DataFactory();
	~DataFactory();

public:
	void Init();
	void Analysis();
	void ReadData();
	void DBStorage();
	void SSDBStorage();
private:
	queue <string> m_FileQueue;
	queue <string> m_DataQueue;

};

