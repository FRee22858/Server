#include "DataFactory.h"
#include "../Utils/FileOperater.h"
#include "../Utils/FolderOperater.h"
#include "../Utils/StrUtil.h"
#include "AnalysisFactory.h"

DataFactory::DataFactory()
{
}


DataFactory::~DataFactory()
{
}

void DataFactory::ReadData()
{
	if (m_FileQueue.empty())
	{
		char * filePath = "E:\\loG";//\\*.log";
		//获取该路径下的所有文件  
		FolderOperater* folder = new FolderOperater();
		folder->GetFiles(filePath, m_FileQueue, "log");
	}
	else
	{
		//frTODO:队列中还有数据暂时不读取新的
	}
	if (!m_FileQueue.empty())
	{
		long size = m_FileQueue.size();
		char* dataBuf = NULL;

		for (int i = 0; i < size; i++)
		{
			FileOperater* file = new FileOperater();
			//char * filePath = "E:\\loG\\MCWY_1012_7_ENTERMAP_2016-12-14-00-00.log";
			if (file->OpenFile(m_FileQueue.front().c_str()))
			{
				int buf_length = 0;
				int str_length;
				while ((str_length = file->GetNextLine(dataBuf, buf_length)) != -1)
				{
					//打印字符串buf，返回字符串长度l和buf分配的字节数i
					printf("str:%s len:%d i:%u\n", (char*)dataBuf, str_length, buf_length);
					m_DataQueue.push(dataBuf);
					free(dataBuf);
					dataBuf = NULL;
				}
			}
			m_FileQueue.pop();
		}
	}
	else
	{
		//frTODO:文件队列为空，说明没有满足条件的文件
	}
}

void DataFactory::Analysis()
{
	if (m_DataQueue.empty())
	{
		//frTODO:数据队列为空
	}
	else
	{
		string strData = m_DataQueue.front();
		strData = StrUtil::Replace(strData, "\n", "");
		vector<string> vecItemData = StrUtil::StrSlipt(strData, "|");
		AnalysisFactory *factory = new AnalysisFactory();
		factory->Init();
		factory->AnalysisData(vecItemData);
		m_DataQueue.pop();
	}

}