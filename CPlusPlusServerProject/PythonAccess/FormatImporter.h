#pragma once
#include <string>
using namespace std;
class FormatImporter
{
public:
	FormatImporter();
	~FormatImporter();
private:
	char* url;
	char* project ;
	char* format_importer ;
public:
	void Init(const char* url, const char* project);
	void Run(const char* event_default, const char* distinct_id_from, const char* timestamp_from, const char* filename);
};

