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
	//文件句柄  
#ifndef _WIN64
	long   hFile = 0;
#else
	intptr_t hFile = 0;
#endif
	//文件信息  
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
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
	//文件句柄  
#ifndef _WIN64
	long   hFile = 0;
#else
	intptr_t hFile = 0;
#endif
	//文件信息  
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
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
					//frTODO:该文件不包含 postfix 后缀
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return true;
}

int FolderOperater::CreateNewFolder(const char* path)  //-1:创建失败 0:创建成功  1:已经存在 
{
	if (_access(path, _A_NORMAL))
	{
		return 1; //已经存在
	}
	else
	{
		return _mkdir(path);
	}
}

int FolderOperater::DeleteFolder(const char* path)  //-1: 删除失败 0:删除成功  1:不存在
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

