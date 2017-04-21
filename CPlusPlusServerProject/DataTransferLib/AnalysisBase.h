#pragma once
#include <string>
#include <vector>
using namespace std;
class AnalysisBase
{
public:
	AnalysisBase();
	virtual ~AnalysisBase();
public:
	virtual void Work(vector<string> vecData);
};

