#include "LogManager.h"


LogManager::LogManager()
{
}


LogManager::~LogManager()
{
}
LogManager *LogManager::m_pInstance = NULL;



//»’÷æ
eLogType LogManager::GetLogType(void)
{
	if (mLogType == "debug")
		return LOG_EVENT_DEBUG;
	else if (mLogType == "warnning")
		return LOG_EVENT_WARNNING;
	else if (mLogType == "error")
		return LOG_EVENT_ERROR;
	else
		return LOG_EVENT_DEBUG;
}

eLogOutPut LogManager::GetOutputFile(void)
{
	if (mLogOutputFile == "screen")
		return LOG_OUTPUT_SCREEN;
	else if (mLogOutputFile == "file")
		return LOG_OUTPUT_FILE;
	else if (mLogOutputFile == "both")
		return LOG_OUTPUT_BOTH;
	else
		return LOG_OUTPUT_UNDEFINE;
}