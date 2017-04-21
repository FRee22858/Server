// CplusplusServerProject.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../PythonAccess/PythonLib.h"
#include <string>

#include "../PythonAccess/FormatImporter.h"

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{

	string url = "http://123.59.209.214:8006/sa";
	string project = "default";
	string format_importer = "..//FormatImporter//format_importer//format_importer.py";
	string event_default = "UserBuy";
	string distinct_id_from = "item_id";
	string timestamp_from = "buy_time";
	string filename = "E://buy.csv";
	PythonLib *python = new PythonLib();
	python->Init();
	python->GetFun();

	////python->format_importer_run(event_default.c_str(), distinct_id_from.c_str(), filename.c_str());

	//string cmd = "python " + format_importer + " csv_event \
	//	         --url " + url + " \
	//			 --project " + project + " \
	//			 --event_default " + event_default + " \
	//			 --distinct_id_from " + distinct_id_from + " \
	//			 --timestamp_from " + timestamp_from + " \
	//			 --filename " + filename + " \
	//			 --debug";

	////string cmd="python 1.py";
	//int n = 3;
	////while (n>0)
	//{
	//	system(cmd.c_str());
	//	n--;
	//}
	//FormatImporter *python = new FormatImporter();
	//python->Init(url.c_str(), project.c_str());
	//python->Run(event_default.c_str(),distinct_id_from.c_str(),timestamp_from.c_str(),filename.c_str());

	system("pause");
	return 0;
}

