#pragma once
#include <queue>

class FolderOperater
{
public:
	FolderOperater();
	~FolderOperater();

public:
	int CreateNewFolder(const char* path);
	int DeleteFolder(const char* path);

	bool OpenOrCreate(const char* path);
	bool OpenExists(const char* path);
	void Close();

	bool GetFiles(const char* path, std::queue<std::string>& files);
	bool GetFiles(const char* path, std::queue <std::string>& files, const char* postfix);

	int  GetSize();
	bool ReSize(int new_size);

	bool IfValid();
	bool operator!() { return IfValid(); }
public:
	static bool RenameFile(const char* old_name, const char* new_name);
	static bool CreateDir(const char* name);
	static bool IfExists(const char* name);
private:
	static unsigned int mFilesCount;
};

