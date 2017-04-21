#include "PythonLib.h"

PythonLib::PythonLib()
{
	// 初始化Python
	// 在使用Python系统前，必须使用Py_Initialize对其
	// 进行初始化。它会载入Python的内建模块并添加系统路
	// 径到模块搜索路径中。这个函数没有返回值，检查系统
	// 是否初始化成功需要使用Py_IsInitialized。
	Py_Initialize();
}
PythonLib::~PythonLib()
{
	Py_Finalize();
}

void PythonLib::Init()
{
	// 初始化Python
	// 在使用Python系统前，必须使用Py_Initialize对其
	// 进行初始化。它会载入Python的内建模块并添加系统路
	// 径到模块搜索路径中。这个函数没有返回值，检查系统
	// 是否初始化成功需要使用Py_IsInitialized。
	Py_Initialize();
}

PyObject* PythonLib::GetFun()
{
	// 检查初始化是否成功
	if (!Py_IsInitialized())
	{
		return NULL;
	}
	// 添加当前路径
	// 把输入的字符串作为Python代码直接运行，返回
	// 表示成功，-1表示有错。大多时候错误都是因为字符串
	// 中有语法错误。
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");

	//frTODO: 加载脚本。获取脚本中函数
	// 载入名为FormatImporter的脚本（注：这里 FormatImporter.py需要在当前目录下）
	PyObject* pModule = NULL;
	pModule = PyImport_ImportModule("FormatImporter");
	if (!pModule)
	{
		printf("can't FormatImporter.py\n");
		return NULL;
	}

	PyObject* pDict = NULL;
	pDict = PyModule_GetDict(pModule);
	if (!pDict)
	{
		return NULL;
	}
	//	PyObject* pFunc = NULL;
	//	PyObject* pArgs = NULL;
	//	PyObject* pRet = NULL;
	//		//// 找出函数名为AddMult的函数
	//		//pFunc = PyDict_GetItemString(pDict, "AddMult");
	//		//if (!pFunc || !PyCallable_Check(pFunc))
	//		//{
	//		//	printf("can't findfunction [AddMult]\n");
	//		//	break;
	//		//}
	//
	//		//pArgs = Py_BuildValue("ii", 12, 14);
	//		//PyObject* pRet = PyEval_CallObject(pFunc, pArgs);
	//		//int a = 0;
	//		//int b = 0;
	//		//if (pRet && PyArg_ParseTuple(pRet, "ii", &a, &b))
	//		//{
	//		//	printf("Function[AddMult] call successful a + b = %d, a * b = %d\n", a, b);
	//		//	nRet = 0;
	//		//}
	//		//if (pArgs)
	//		//	Py_DECREF(pArgs);
	//		//if (pFunc)
	//		//	Py_DECREF(pFunc);
	//		// 找出函数名为HelloWorld的函数
	//		pFunc = PyDict_GetItemString(pDict, "HelloWorld");
	//		if (!pFunc || !PyCallable_Check(pFunc))
	//		{
	//			printf("can't findfunction [HelloWorld]\n");
	//			break;
	//		}
	//		pArgs = Py_BuildValue("(s)", "magictong");
	//		PyEval_CallObject(pFunc, pArgs);
	//	if (pRet)		Py_DECREF(pRet);
	//	if (pArgs)		Py_DECREF(pArgs);
	//	if (pFunc)		Py_DECREF(pFunc);
	//	if (pDict)		Py_DECREF(pDict);
	//	if (pModule)	Py_DECREF(pModule);
	//	if (pName)		Py_DECREF(pName);
	//
	
	return NULL;

}

//void PythonLib::Run()
//{
//	int nRet = -1;
//	PyObject* pName = NULL;
//	PyObject* pModule = NULL;
//	PyObject* pDict = NULL;
//	PyObject* pFunc = NULL;
//	PyObject* pArgs = NULL;
//	PyObject* pRet = NULL;
//	do
//	{
//		// 初始化Python
//		// 在使用Python系统前，必须使用Py_Initialize对其
//		// 进行初始化。它会载入Python的内建模块并添加系统路
//		// 径到模块搜索路径中。这个函数没有返回值，检查系统
//		// 是否初始化成功需要使用Py_IsInitialized。
//		Py_Initialize();
//
//		// 检查初始化是否成功
//		if (!Py_IsInitialized())
//		{
//			break;
//		}
//		// 添加当前路径
//		// 把输入的字符串作为Python代码直接运行，返回
//		// 表示成功，-1表示有错。大多时候错误都是因为字符串
//		// 中有语法错误。
//		PyRun_SimpleString("import sys");
//		PyRun_SimpleString("sys.path.append('./')");
//
//		// 载入名为PyPlugin的脚本
//
//		//pName = PyString_FromString("E:\visual studio 2013\Projects\CPlusPlusServerProject\FormatImporter\FormatImporter.py");
//		//pModule = PyImport_Import(pName);
//
//		pModule = PyImport_ImportModule("FormatImporter");
//		//pModule = PyImport_ImportModule("format_importer");
//		if (!pModule)
//		{
//			printf("can't findPyPlugin.py\n");
//			break;
//		}
//
//		pDict = PyModule_GetDict(pModule);
//		if (!pDict)
//		{
//			break;
//		}
//
//		//// 找出函数名为AddMult的函数
//		//pFunc = PyDict_GetItemString(pDict, "AddMult");
//		//if (!pFunc || !PyCallable_Check(pFunc))
//		//{
//		//	printf("can't findfunction [AddMult]\n");
//		//	break;
//		//}
//
//		//pArgs = Py_BuildValue("ii", 12, 14);
//		//PyObject* pRet = PyEval_CallObject(pFunc, pArgs);
//		//int a = 0;
//		//int b = 0;
//		//if (pRet && PyArg_ParseTuple(pRet, "ii", &a, &b))
//		//{
//		//	printf("Function[AddMult] call successful a + b = %d, a * b = %d\n", a, b);
//		//	nRet = 0;
//		//}
//		//if (pArgs)
//		//	Py_DECREF(pArgs);
//		//if (pFunc)
//		//	Py_DECREF(pFunc);
//		// 找出函数名为HelloWorld的函数
//		pFunc = PyDict_GetItemString(pDict, "HelloWorld");
//		if (!pFunc || !PyCallable_Check(pFunc))
//		{
//			printf("can't findfunction [HelloWorld]\n");
//			break;
//		}
//		pArgs = Py_BuildValue("(s)", "magictong");
//		PyEval_CallObject(pFunc, pArgs);
//	} while (0);
//
//	if (pRet)		Py_DECREF(pRet);
//	if (pArgs)		Py_DECREF(pArgs);
//	if (pFunc)		Py_DECREF(pFunc);
//	if (pDict)		Py_DECREF(pDict);
//	if (pModule)	Py_DECREF(pModule);
//	if (pName)		Py_DECREF(pName);
//
//	Py_Finalize();
//}
//
//int PythonLib::Run1()
//{
//	//在调用Python的提供的给C的API之前，通过执行初始化
//	//来添加Python的内建模块、__main__、sys等
//	Py_Initialize();
//
//	//检查初始化是否完成
//	if (!Py_IsInitialized())
//	{
//		return -1;
//	}
//	//第二步：导入sys模块
//	PyRun_SimpleString("import sys");
//	PyRun_SimpleString("sys.path.append('./')");
//	//第三步：导入执行脚本时的命令行参数，如：./sample.py arg1 arg2
//	//int argc = 2;
//	//char *argv[2];
//	//argv[0] = "arg1";
//	//argv[1] = "arg2";
//	//PySys_SetArgv(argc,(wchar_t**)argv);
//
//	//第四步：执行调用脚本文件命令,注意文件的路径
//	//if (PyRun_SimpleString("execfile(r'./sample.py')") == NULL)
//	//{
//	//	return -1;
//	//}
//	//if (PyRun_SimpleString("with open('../FormatImporter/sample.py','r') as f: exec(f.read())") == NULL)
//	if (PyRun_SimpleString("with open('../FormatImporter/format_importer/format_importer.py --url http://123.59.209.214:8006/sa','r') as f: exec(f.read())") == NULL)
//	{
//		return -1;	
//	}
//
//	//第五步：关闭Python解释器
//	Py_Finalize();
//	return 0;
//}
//
//void PythonLib::printDict(PyObject* obj)
//{
//	if (!PyDict_Check(obj))
//		return;
//
//	PyObject *k, *keys;
//	keys = PyDict_Keys(obj);
//
//	for (int i = 0; i < PyList_GET_SIZE(keys); i++)
//	{
//		k = PyList_GET_ITEM(keys, i);
//		//char *c_name = PyString_AsString(k);
//		char * my_result = NULL;
//		if (PyBytes_Check(k)) {
//			my_result = PyBytes_AS_STRING(k); // Borrowed pointer
//			my_result = strdup(my_result);
//		}
//		printf("%s\n", my_result);
//	}
//}
//
//int PythonLib::Run2()
//{
//	Py_Initialize();
//	if (!Py_IsInitialized())
//		return -1;
//
//	PyRun_SimpleString("import sys");
//	//where you put your py file
//	PyRun_SimpleString("sys.path.append('.')");
//	//Import 
//	PyObject* pModule = PyImport_ImportModule("pc");//no *.py
//	if (!pModule)
//	{
//		printf("can't open python file!\n");
//		return -1;
//	}
//	//Dict module
//	PyObject* pDict = PyModule_GetDict(pModule);
//	if (!pDict)
//	{
//		printf("can't find dictionary.\n");
//		return -1;
//	}
//	printDict(pDict);
//
//	PyObject *pFunHi = PyDict_GetItemString(pDict, "sayHi");
//	PyObject_CallFunction(pFunHi, "s", "yourname");// @para1: obj, @para2: name
//	//release
//	Py_DECREF(pFunHi);
//
//	//Contruct a obj , call it's function
//	//Second Class
//	PyObject *pClassSecond = PyDict_GetItemString(pDict, "Second");
//	if (!pClassSecond)
//	{
//		printf("can't find second class.\n");
//		return -1;
//	}
//	//Person Class
//	PyObject *pClassPerson = PyDict_GetItemString(pDict, "Person");
//	if (!pClassPerson)
//	{
//		printf("can't find Person class.\n");
//		return -1;
//	}
//	////Construct Second Instance
//	//PyObject* pInstanceSecond = PyInstance_New(pClassSecond, NULL, NULL);
//	//if (!pInstanceSecond)
//	//{
//	//	printf("can't create second instance.\n");
//	//	return -1;
//	//}
//	////Construct Person Instance
//	//PyObject *pInstancePerson = PyInstance_New(pClassPerson, NULL, NULL);
//	//if (!pInstancePerson)
//	//{
//	//	printf("can't create Person instance.\n");
//	//	return -1;
//	//}
//
//	////pass Person Instance to Second Instance 
//	//PyObject_CallMethod(pInstanceSecond, "invoke", "O", pInstancePerson);
//	//PyObject_CallMethod(pInstanceSecond, "method2", "O", pInstancePerson);
//
//	//call execute a py file
//	PyRun_SimpleString("exec(compile(open('./1.py').read(),'1.py', 'exec'),locals(), globals())");
//	//release
//	//Py_DECREF(pInstanceSecond);
//	//Py_DECREF(pInstancePerson);
//	Py_DECREF(pClassSecond);
//	Py_DECREF(pClassPerson);
//	Py_DECREF(pModule);
//	Py_Finalize();
//
//	return 0;
//}
//
//void PythonLib::RunTest()
//{
//	int nRet = -1;
//	PyObject* pName = NULL;
//	PyObject* pModule = NULL;
//	PyObject* pDict = NULL;
//	PyObject* pFunc = NULL;
//	PyObject* pArgs = NULL;
//	PyObject* pRet = NULL;
//	do
//	{
//		Py_Initialize();
//		if (!Py_IsInitialized())
//		{
//			break;
//		}
//		// 添加当前路径
//		// 把输入的字符串作为Python代码直接运行，返回
//		// 表示成功，-1表示有错。大多时候错误都是因为字符串
//		// 中有语法错误。
//		PyRun_SimpleString("import sys");
//		PyRun_SimpleString("sys.path.append('./')");
//
//		// 载入名为PyPlugin的脚本
//
//		pModule = PyImport_ImportModule("format_importer");
//		if (!pModule)
//		{
//			printf("can't findPyPlugin.py\n");
//			break;
//		}
//
//		pDict = PyModule_GetDict(pModule);
//		if (!pDict)
//		{
//			break;
//		}
//
//		// 找出函数名为main的函数
//		pFunc = PyDict_GetItemString(pDict, "main");
//		if (!pFunc || !PyCallable_Check(pFunc))
//		{
//			printf("can't findfunction [main]\n");
//			break;
//		}
//		pArgs = Py_BuildValue("(s)", "magictong");
//		PyEval_CallObject(pFunc, pArgs);
//	} while (0);
//
//	if (pRet)		Py_DECREF(pRet);
//	if (pArgs)		Py_DECREF(pArgs);
//	if (pFunc)		Py_DECREF(pFunc);
//	if (pDict)		Py_DECREF(pDict);
//	if (pModule)	Py_DECREF(pModule);
//	if (pName)		Py_DECREF(pName);
//
//	Py_Finalize();
//}

