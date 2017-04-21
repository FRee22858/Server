#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_
///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TypeDef.h
// 创建者：FRee
// 创建时间：2017-04-06
// 内容描述：自定义类型定义，建议多处使用的自定义类型统一在此处定义
///////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <list>
#include <vector>
#include <map>

//日志类型定义
typedef enum eLogType              
{
	LOG_EVENT_DEBUG = 0,		//调试信息
	LOG_EVENT_WARNNING = 1,		//警告信息
	LOG_EVENT_ERROR = 2,		//错误信息
	LOG_EVENT_INFO = 3			//输出信息
}enumLogType;

//日志输出类型
typedef enum eLogOutPut
{
	LOG_OUTPUT_UNDEFINE = 0,	//未定义输出类型
	LOG_OUTPUT_SCREEN	= 1,	//输出到屏幕
	LOG_OUTPUT_FILE		= 2,	//输出到文件
	LOG_OUTPUT_BOTH		= 3		//输出到屏幕和文件
}enumLogOutPut;

//模块类型定义
typedef enum eModuleType
{
	NONE = 0,	//无类型，属于错误
	OTHER = 1,	//其他
	DB = 2,		//数据库
	SSDB=3,		//神策数据	
	//frTODO: 功能模块类型添加
}enumModuleType;

#endif
