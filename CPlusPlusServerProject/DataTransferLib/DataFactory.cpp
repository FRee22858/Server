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
		//��ȡ��·���µ������ļ�  
		FolderOperater* folder = new FolderOperater();
		folder->GetFiles(filePath, m_FileQueue, "log");
	}
	else
	{
		//frTODO:�����л���������ʱ����ȡ�µ�
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
					//��ӡ�ַ���buf�������ַ�������l��buf������ֽ���i
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
		//frTODO:�ļ�����Ϊ�գ�˵��û�������������ļ�
	}
}

void DataFactory::Analysis()
{
	if (m_DataQueue.empty())
	{
		//frTODO:���ݶ���Ϊ��
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