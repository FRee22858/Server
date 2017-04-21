#pragma once
#include <string>
#include <vector>
#include <map>
#include "AnalysisBase.h"
using namespace std;

typedef enum dataModeType
{
	CONSUME = 0,
	OBTAIN = 1,
	CREATECHAR = 2,
	LOGIN = 3,
	LOGOUT = 4,
	RECHARGE = 5,
	TASK = 6,
	ARENA = 7,
	ENTERMAP = 8,
	QUITMAP = 9,
	ONLINE = 10,
	LISTENCHAT = 11
}DataModeType;

class AnalysisFactory
{
public:
	AnalysisFactory();
	~AnalysisFactory();
private:
	map<string, DataModeType> _mapModeDiverter;
	AnalysisBase *_pAnalysis;
public:
	void Init();
	void AnalysisData(vector<string> vecData);
	void Exit();
};

