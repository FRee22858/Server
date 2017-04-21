// My Server.cpp : 定义控制台应用程序的入口点。
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
	//打开Lua  
	lua_State *L = luaL_newstate();
	/*加载lua所有库*/
	luaL_openlibs(L);
	//luaopen_base(L);    
	//luaopen_io(L);                //替换老版本的lua_iolibopen(L);    
	//luaopen_string(L);            //替换老版本的lua_strlibopen(L);    
	//luaopen_math(L);              //替换老版本的lua_mathlibopen(L);    
	//luaopen_debug(L);             //替换老版本的lua_dblibopen(L);    
	/*加载test.lua文件并运行*/
	luaL_dofile(L, "main.lua");
	/*清除lua*/
	lua_close(L);
	getchar();

	return 0;
}


