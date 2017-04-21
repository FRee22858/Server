#pragma once
#include "Python35\include\Python.h"
#include <string>
using namespace std;

class PythonLib
{
public:
	PythonLib();
	~PythonLib();
public:
	void Init();
	PyObject* GetFun();

//public:
//	void Run();
//	int Run1();
//	int Run2();
//	void printDict(PyObject* obj);
//	void RunTest();
};

