// My Server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "stdafx.h"  
#include <iostream>  
#include <stdlib.h>  
#include <stdio.h>  
#include <string.h>  

extern "C"
{
#include "lua.h"  
#include "lualib.h"  
#include "lauxlib.h"  
};
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	//��Lua  
	lua_State *L = luaL_newstate();
	/*����lua���п�*/
	luaL_openlibs(L);
	//luaopen_base(L);    
	//luaopen_io(L);                //�滻�ϰ汾��lua_iolibopen(L);    
	//luaopen_string(L);            //�滻�ϰ汾��lua_strlibopen(L);    
	//luaopen_math(L);              //�滻�ϰ汾��lua_mathlibopen(L);    
	//luaopen_debug(L);             //�滻�ϰ汾��lua_dblibopen(L);    
	/*����test.lua�ļ�������*/
	luaL_dofile(L, "main.lua");
	/*���lua*/
	lua_close(L);
	getchar();

	return 0;
}


