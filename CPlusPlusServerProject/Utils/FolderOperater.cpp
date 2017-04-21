#include "FolderOperater.h"
#include <io.h>
#include <direct.h>  
#include <algorithm>  

FolderOperater::FolderOperater()
{
}


FolderOperater::~FolderOperater()
{
	
}


bool FolderOperater::GetFiles(const char* path, std::queue <std::string>& files)
{
	//�ļ����  
#ifndef _WIN64
	long   hFile = 0;
#else
	intptr_t hFile = 0;
#endif
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					GetFiles(p.assign(path).append("\\").append(fileinfo.name).c_str(), files);
				}
			}
			else
			{
				files.push(p.assign(path).append("\\").append(fileinfo.name).c_str());
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return true;
}

bool FolderOperater::GetFiles(const char* path, std::queue <std::string>& files, const char* postfix)
{
	//�ļ����  
#ifndef _WIN64
	long   hFile = 0;
#else
	intptr_t hFile = 0;
#endif
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					GetFiles(p.assign(path).append("\\").append(fileinfo.name).c_str(), files);
				}
			}
			else
			{
				const char* pos = strrchr(fileinfo.name, '.');
				if (pos)
				{
					std::string str(pos + 1);
					std::transform(str.begin(), str.end(), str.begin(), ::towlower);
					if (strcmp(str.c_str(), postfix))
					{
						files.push(p.assign(path).append("\\").append(fileinfo.name).c_str());
					}
				}
				else
				{
					//frTODO:���ļ������� postfix ��׺
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return true;
}

int FolderOperater::CreateNewFolder(const char* path)  //-1:����ʧ�� 0:�����ɹ�  1:�Ѿ����� 
{
	if (_access(path, _A_NORMAL))
	{
		return 1; //�Ѿ�����
	}
	else
	{
		return _mkdir(path);
	}
}

int FolderOperater::DeleteFolder(const char* path)  //-1: ɾ��ʧ�� 0:ɾ���ɹ�  1:������
{
	if (_access(path, _A_NORMAL))
	{
		return _rmdir(path);
	}
	else
	{
		return 1;
	}
}

