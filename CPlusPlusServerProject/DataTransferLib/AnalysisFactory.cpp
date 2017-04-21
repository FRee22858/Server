#include "AnalysisFactory.h"
#include "AnalysisBase.h"
#include "Analysis_CharBaseAction.h"

AnalysisFactory::AnalysisFactory()
{
}

AnalysisFactory::~AnalysisFactory()
{
	if (_mapModeDiverter.empty())
	{

	}
	else
	{
		_mapModeDiverter.clear();
	}
}

void AnalysisFactory::Init()
{
	if (_mapModeDiverter.empty())
	{

	}
	else
	{
		_mapModeDiverter.clear();
	}
	_mapModeDiverter["CONSUME"] = CONSUME;
	_mapModeDiverter["OBTAIN"] = OBTAIN;
	_mapModeDiverter["CREATECHAR"] = CREATECHAR;
	_mapModeDiverter["LOGIN"] = LOGIN;
	_mapModeDiverter["LOGOUT"] = LOGOUT;
	_mapModeDiverter["RECHARGE"] = RECHARGE;
	_mapModeDiverter["TASK"] = TASK;
	_mapModeDiverter["ARENA"] = ARENA;
	_mapModeDiverter["ENTERMAP"] = ENTERMAP;
	_mapModeDiverter["QUITMAP"] = QUITMAP;
	_mapModeDiverter["ONLINE"] = ONLINE;
	_mapModeDiverter["LISTENCHAT"] = LISTENCHAT;
}

void AnalysisFactory::AnalysisData(vector<string> vecData)
{
	string Key = vecData[0];
	switch (_mapModeDiverter[Key])
	{
	case CONSUME:
		break;
	case OBTAIN:
		break;
	case CREATECHAR:
		break;
	case LOGIN:
	case LOGOUT:
	case RECHARGE:
	case TASK:
	case ARENA:
	case ENTERMAP:
	case QUITMAP:
	case ONLINE:
	case LISTENCHAT:
		_pAnalysis = new Analysis_CharBaseAction();
		break;
	default:
		//frTODO£º´íÎóÊý¾Ý
		return;
	}
	_pAnalysis->Work(vecData);
	return;
}


void AnalysisFactory::Exit()
{
	if (_mapModeDiverter.empty())
	{

	}
	else
	{
		_mapModeDiverter.clear();
	}
}