#include "FormatImporter.h"


FormatImporter::FormatImporter()
{
	url = "http://123.59.209.214:8006/sa";
	project = "default";
	format_importer = "..//FormatImporter//format_importer//format_importer.py";
}

FormatImporter::~FormatImporter()
{
}

void FormatImporter::Init(const char* url, const char* project)
{
	url = url;
	project = project;
	format_importer = "..//FormatImporter//format_importer//format_importer.py";
}
void FormatImporter::Run(const char*event_default, const char* distinct_id_from, const char*timestamp_from, const char*filename)
{
	string cmd = "python " + string(format_importer) + " csv_event \
								--url " + url + " \
								--project " + project + " \
								--event_default " + event_default + " \
								--distinct_id_from " + distinct_id_from + " \
								--timestamp_from " + timestamp_from + " \
							    --filename " + filename +
							    " --debug";
	system(cmd.c_str());
}