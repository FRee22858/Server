#pragma once
#include "AnalysisBase.h"
class Analysis_CharBaseAction :
	public AnalysisBase
{
public:
	Analysis_CharBaseAction();
	~Analysis_CharBaseAction();
public:
	void Work(vector<string> vecData);
};

